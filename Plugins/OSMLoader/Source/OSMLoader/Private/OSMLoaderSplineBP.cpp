#include "OSMLoaderSplineBP.h"

/*

		##### CLASS NOT IN USE #####

*/

UOSMLoaderSplineBP::UOSMLoaderSplineBP(const class FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	
}

//Happy Message
FString UOSMLoaderSplineBP::GetHappyMessage()
{
	return FString("Victory! Victory BP Library Works!");
}

void UOSMLoaderSplineBP::SaveStringTextToFile(
	FString SaveDirectory, 
	FString FileName, 
	FString SaveText,
	bool AllowOverWriting
){
	return;
}


