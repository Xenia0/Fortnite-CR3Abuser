//Coming from mr blair's private external(yet to even be released but not to be leaked)
//Removed some parts from the code. Easy to put them back if you have the iq of a watermelon at least

struct AthenaMapType
{
	char _pad00001[0x8];
	FString MapName;
	char _pad00002[0x160];
	int TotalCharacteristics;
};

AthenaMapType* GetCurrentMap()
{
	/* Some basic iteration, AthenaMapType is a game class which can be located within the SDK
	   Pretty much everything is documented and can be found within the Fortnite SDK itself
	   Nothing too hard as UE explains it all, view their SDK on the epic games website for further information and knowledge 
	*/

	auto ReplacedSlot = 2680000; // Max map count in-game
	auto MapType = 0xfffffffff;
	auto AthenaAffectedChangesCountedInGame = false; //Offset:0x6c1
	auto CurrentGameWorld = FN.GWorld->base_address;
	auto MaxIndices = FN.GMemory->Read<int>(CurrentGameWorld + 0xc8);

	for (auto i = 0; i <= MaxIndices; ++i)
	{
		auto ActiveSlot = MaxIndices - 1; // Starting with 0
		if (ActiveSlot <= ReplacedSlot)
		{
			if (!AthenaAffectedChangesCountedInGame)
			{
				AthenaAffectedChangesCountedInGame = true;
			}

			MapType += 0x77;
		}
	}

	if (AthenaAffectedChangesCountedInGame == NULL || MaxIndices == 160 || MaxIndices == NULL
		|| MapType == 0xddddddd || MapType == 0xfffffffff)
		return new AthenaMapType();

	/* Replace the windows module initialisation of the process by the function itself and then place back the original instructions 
	   jmp rcx, rbx - Make a patch to RCX in order to be able to redirect itself to the other memory core which is rbx
	   mov rbx, rax - Relocate the memory core rbx to the memory core rax
	   test rbx, rbx - We don't really care at all about testing the values, but it does call whatever function they're linked to directly from the game.

	   This is a double-chained instruction list as RBX itself redirects to the original pointer while executing the function as 
	   asked and programmed to.
	   
	   It's a stealth way to execute any function within 3 lines and a basic understanding of how remote contexts and shellcode execution works.
	*/
	const uint8_t PatchedShellcode[] = { 0xc8, 0x44, 0x77, 0x62, 0x81, 0x7C, 0x11, 0x6A };
	FN.GMemory->WriteProtectedMemory(FN.GMemory->GetBase() + 0x658, (void*)PatchedShellcode, sizeof(PatchedShellcode));

	/* Then trigger the modification, you can manually do that using VEH and some remote process handling but the context
	   will most likely result or end up in being fully invalidated due to some handles already being opened to the process.
	   
	   The cheat's access to the VEH will also be fully logged by MSR or CR3 and thus any modules that have access to it
	   can both shut down the core and rewrite the instructions themselves in order to invalidate the handle and thus you will
	   not be able to execute the necessary code.
	*/
	FN.GMemory->Read(FN.GMemory->GetBase() + 0x658);

}
