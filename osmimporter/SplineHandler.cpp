#include "SplineHandler.h"

/* COPYRIGHT NOTICE
   the following code is ported from Jonas Treumer's rovermaprenderer framework which is
   part of the FwA 16/2 project at TU Bergakademie Freiberg



   Copyright (c) 2018 Jonas Treumer

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
   associated documentation files (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial
   portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/



SplineHandler::VertexData SplineHandler::getMeshFromSplinePoints(std::vector<OSMImporter::Vec3> splinePoints, OSMImporter::EntityType entityType,
                                                                 std::vector<float>& heightValues, long startX, long startY) {
    VertexData vt;

    //There must be at least two coordinate pairs:
    if (splinePoints.size() < 2) {
        return vt;
    }

    double halfThicknessRSMc = 1.0;

    if (entityType == OSMImporter::EntityType_POLYLINE_HIGHWAY_DEFAULT) {
        halfThicknessRSMc = 1.0;
    } else if (entityType == OSMImporter::EntityType_POLYLINE_HIGHWAY_PRIMARY || entityType == OSMImporter::EntityType_POLYLINE_HIGHWAY_SECONDARY) {
        halfThicknessRSMc = 4.5;
    } else if (entityType == OSMImporter::EntityType_POLYLINE_HIGHWAY_RESIDENTIAL) {
        halfThicknessRSMc = 3.0;
    } else if (entityType == OSMImporter::EntityType_POLYLINE_HIGHWAY_SERVICE) {
        halfThicknessRSMc = 2.0;
    } else if (entityType == OSMImporter::EntityType_POLYLINE_HIGHWAY_FOOTWAY) {
        halfThicknessRSMc = 1.0;
    } else if (entityType == OSMImporter::EntityType_POLYLINE_HIGHWAY_CYCLEWAY) {
        halfThicknessRSMc = 1.0;
    } else if (entityType == OSMImporter::EntityType_POLYLINE_RIVER) {
        halfThicknessRSMc = 4.5;
    } else if (entityType == OSMImporter::EntityType_POLYLINE_STREAM) {
        halfThicknessRSMc = 1.0;
    }

    //Always keep the last gradient:
    std::vector<double> lastPositionRSMc2;
    std::vector<double> lastGradientRSMc2;

    //Manage structure texture coordinate:
    double structureTex = 0;
    bool structureTexFlip = false;

    double sumTex = 0.0;

    //Iterate over the coordinates:
    bool firstOfPrimitive = true;
    unsigned int positionIndex = 0;

    while (positionIndex < splinePoints.size()) {
        OSMImporter::Vec3 point = splinePoints[positionIndex];

        //Get the current point as vector:
        std::vector<double> positionRSMc2 = { point.x(), point.y() };

        //If we are not at the last point, there is a current gradient.
        //Calculate it - always normalized.
        std::vector<double> currentGradientRSMc2;
        if (positionIndex != (splinePoints.size() - 1)) {
            OSMImporter::Vec3 nextPoint = splinePoints[positionIndex + 1];

            //If the next point is equal to the current one, we omit this one.
            if (point.x() == nextPoint.x() && point.y() == nextPoint.y() && point.z() == nextPoint.z()) {
                positionIndex++;
                continue;
            }

            std::vector<double> nextPositionRSMc2 = { nextPoint.x(), nextPoint.y() };
            std::vector<double> subtrVector = { nextPositionRSMc2[0] - positionRSMc2[0], nextPositionRSMc2[1] - positionRSMc2[1] };
            double length = sqrt(subtrVector[0] * subtrVector[0] + subtrVector[1] * subtrVector[1]);
            currentGradientRSMc2 = { subtrVector[0] / length, subtrVector[1] / length };
        }

        //If last and current gradient are both nil, all points have been equal and this is the last one.
        //Ignore that case, too.
        if ((lastGradientRSMc2.empty()) && (currentGradientRSMc2.empty())) {
            positionIndex++;
            continue;
        }

        //Texture coordinate:
        //If there is no last vector, we are at the beginning and keep the zero.
        //If there is, we calculate the difference.
        if (!lastPositionRSMc2.empty()) {
            //Add the Euclidean distance to the last vertex to the texture coordinate:
            structureTex += sqrt(std::pow(positionRSMc2[0] - lastPositionRSMc2[0], 2.0) + pow(positionRSMc2[1] - lastPositionRSMc2[1], 2.0));
        }

        std::vector<double> leftLowerStructureTex2 = { structureTex, structureTex };
        std::vector<double> rightLowerStructureTex2 = { structureTex, structureTex };

        //Calculate the correction vector:
        std::vector<double> lowerCorrectionRSMc2 = { 0, 0 };

        //The correction can decide to restart the line.
        //This should be done if the turn is too pointy.
        bool restartLine = false;

        //If we have both last and current gradient, we do the trapezoid correction:
        if ((!lastGradientRSMc2.empty()) && (!currentGradientRSMc2.empty())) {
            //Calculate the determinant:
            bool turnLeft = (lastGradientRSMc2[0] * currentGradientRSMc2[1]) - (lastGradientRSMc2[1] * currentGradientRSMc2[0]) < 0;

            //Calculate gamma (hopefully small angle between the two span vectors):
            double gamma = std::acos((lastGradientRSMc2[0] * currentGradientRSMc2[0] + lastGradientRSMc2[1] * currentGradientRSMc2[1]));

            //acos will never return Inf or NaN in that case, right?
            //So we only check that the angle is not > gammaLimit.
            if (gamma <= SplineHandler::gammaLimit) {
                //Calculate s:
                double sLower = halfThicknessRSMc * tan(gamma / 2);

                //Scale the last gradient with s:
                lowerCorrectionRSMc2 = { sLower * lastGradientRSMc2[0], sLower * lastGradientRSMc2[1]};

                //Derive the sign from the determinant:
                //Negative -> turn to left.
                if (turnLeft) {
                    //Adjust structure texture coordinatesRSMc2 on right hand.
                    //First index for this primitive.
                    //Second index for next primitive.
                    lowerCorrectionRSMc2[0] *= -1;
                    lowerCorrectionRSMc2[1] *= -1;
                }
            } else {
                //Too pointy. Restart the line.
                restartLine = true;
            }
        }

        //Use one gradient that is not nil for arranging the points:
        std::vector<double> gradientRSMc2;
        if (!lastGradientRSMc2.empty()) {
            gradientRSMc2 = lastGradientRSMc2;
        } else {
            gradientRSMc2 = currentGradientRSMc2;
        }

        //Calculate the right-hand normal vector (which is a left-hand normal because of negative Y-up in RSMc).
        //We can use the Orth property because the gradient is already normalized.
        //Scale it with the half thickness:
        std::vector<double> lowerScaledNormalRSMc2 = { gradientRSMc2[1] * halfThicknessRSMc, -gradientRSMc2[0] * halfThicknessRSMc };

        //Use it to calculate fromLeftPt and right point.
        //Apply the correction vector to both.
        std::vector<double> leftLowerRSMc2 = { positionRSMc2[0] + lowerScaledNormalRSMc2[0] + lowerCorrectionRSMc2[0], positionRSMc2[1] + lowerScaledNormalRSMc2[1] + lowerCorrectionRSMc2[1] };
        std::vector<double> rightLowerRSMc2 = { positionRSMc2[0] - lowerScaledNormalRSMc2[0] - lowerCorrectionRSMc2[0], positionRSMc2[1] - lowerScaledNormalRSMc2[1] - lowerCorrectionRSMc2[1] };

        //Generate vertex data for the two points and for upper and lower.
        //Lower can be omitted if fillFactor is 1.0.
        vt.vertices.emplace_back(OSMImporter::Vec3(leftLowerRSMc2[0], leftLowerRSMc2[1], OSMHelper::getHeightAtPosition(leftLowerRSMc2[0], leftLowerRSMc2[1], heightValues, startX, startY) + 0.1));
        vt.vertices.emplace_back(OSMImporter::Vec3(rightLowerRSMc2[0], rightLowerRSMc2[1], OSMHelper::getHeightAtPosition(leftLowerRSMc2[0], leftLowerRSMc2[1], heightValues, startX, startY) + 0.1));

        if(firstOfPrimitive) {
            vt.uvs.emplace_back(OSMImporter::Vec2(0, 0));
            vt.uvs.emplace_back(OSMImporter::Vec2(1, 0));
        } else {
            OSMImporter::Vec3 currentPoint = splinePoints[positionIndex];
            OSMImporter::Vec3 lastPoint = splinePoints[positionIndex - 1];

            double delta = sqrt(pow(currentPoint.x() - lastPoint.x(), 2) + pow(currentPoint.y() - lastPoint.y(), 2)) / 4.0;

            sumTex += delta;

            vt.uvs.emplace_back(OSMImporter::Vec2(0, sumTex));
            vt.uvs.emplace_back(OSMImporter::Vec2(1, sumTex));
        }

        if(!firstOfPrimitive) {
            vt.triangles.emplace_back(2 * positionIndex);
            vt.triangles.emplace_back(2 * positionIndex - 1);
            vt.triangles.emplace_back(2 * positionIndex - 2);

            vt.triangles.emplace_back(2 * positionIndex - 1);
            vt.triangles.emplace_back(2 * positionIndex);
            vt.triangles.emplace_back(2 * positionIndex + 1);
        }


        //Definitely done with first primitive:
        firstOfPrimitive = false;

        //Flip structure texture coordinatesRSMc2 index:
        structureTexFlip = !structureTexFlip;

        //Only remember the last gradient if we do not restart the line:
        if (restartLine) {
            //No last variables:
            lastPositionRSMc2.clear();
            lastGradientRSMc2.clear();

            //We start with the first primitive -> degenerated triangles:
            firstOfPrimitive = true;

            //Do not increment the index at this point so we visit the node again!
        } else {
            //Shift to the current variables:
            lastPositionRSMc2 = positionRSMc2;
            lastGradientRSMc2 = currentGradientRSMc2;

            //Increment the index:
            positionIndex++;
        }
    }

    return vt;
}