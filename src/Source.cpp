
#include <fstream>
#include <iostream>
#include <vector>
#include <optional>
#include "Bones.h"
#include "Types.h"


// Helper
bool CompareFloat(float a, float b, float threshold = 0.0001f)
{
	return fabs(a - b) > threshold;
}

std::optional<std::shared_ptr<std::vector<char>>> ReadToVector(std::ifstream& InStream)
{
	if(InStream)
	{
		// Get Size
		InStream.seekg(0, std::ios::end);
		auto FileSize = InStream.tellg();
		InStream.seekg(0, std::ios::beg);

		// Create Vector
		std::shared_ptr<std::vector<char>> Data = std::make_shared<std::vector<char>>();
		Data->resize(FileSize);

		

		//std::vector<unsigned char>
		InStream.read(Data->data(), FileSize);

		return Data;
	}

	return std::nullopt;
}

uint64_t ReadStringEntry(const char* dataPtr, uint64_t index,  StringEntries& BlockPtr)
{
	// Read BoneNames
	BlockPtr.EntryCount = ToLittle32(dataPtr + index);
	BlockPtr.Entries.resize(BlockPtr.EntryCount);
	index += 4;

	for (uint32_t i = 0; i < BlockPtr.EntryCount; ++i)
	{
		auto StringLength = ToLittle32(dataPtr + index);
		index += 4;
		BlockPtr.Entries[i].StrLen = StringLength;
		BlockPtr.Entries[i].Src = dataPtr + index;
		index += StringLength;
	}

	return index;
}

uint64_t WriteStringEntry(char* dataPtr, uint64_t index, StringEntries& BlockPtr)
{
	// Start with the header value
	WriteBig32(dataPtr + index, BlockPtr.EntryCount);
	index += 4;

	for (uint32_t i = 0; i < BlockPtr.EntryCount; ++i)
	{
		WriteBig32(dataPtr + index, BlockPtr.Entries[i].StrLen);
		index += 4;
		memcpy(dataPtr + index, BlockPtr.Entries[i].Src, BlockPtr.Entries[i].StrLen);
		index += BlockPtr.Entries[i].StrLen;
	}

	return index;
}

uint64_t ReadShortEntry(const char* dataPtr, uint64_t index, ShortEntries& BlockPtr)
{
	// Read BoneNames
	BlockPtr.EntryCount = ToLittle32(dataPtr + index);
	BlockPtr.Entries.resize(BlockPtr.EntryCount);
	index += 4;

	for (uint32_t i = 0; i < BlockPtr.EntryCount; ++i)
	{
		BlockPtr.Entries[i] = ToLittle16(dataPtr + index);
		index += 2;
	}

	return index;
}

uint64_t WriteShortEntry(char* dataPtr, uint64_t index, ShortEntries& BlockPtr)
{
	// Start with the header value
	WriteBig32(dataPtr + index, BlockPtr.EntryCount);
	index += 4;

	for (uint32_t i = 0; i < BlockPtr.EntryCount; ++i)
	{
		WriteBig16(dataPtr + index, BlockPtr.Entries[i]);
		index += 2;
	}

	return index;
}

uint64_t ReadFloatEntry(const char* dataPtr, uint64_t index, FloatEntries& BlockPtr)
{
	// Read BoneNames
	BlockPtr.EntryCount = ToLittle32(dataPtr + index);
	BlockPtr.Entries.resize(BlockPtr.EntryCount);
	index += 4;

	for (uint32_t i = 0; i < BlockPtr.EntryCount; ++i)
	{
		auto LocalFloatAsInt = ToLittle32(dataPtr + index);

		BlockPtr.Entries[i] = *(reinterpret_cast<float*>(&LocalFloatAsInt));
		index += 4;
	}

	return index;
}

uint64_t WriteFloatEntry(char* dataPtr, uint64_t index, FloatEntries& BlockPtr)
{
	// Start with the header value
	WriteBig32(dataPtr + index, BlockPtr.EntryCount);
	index += 4;

	for (uint32_t i = 0; i < BlockPtr.EntryCount; ++i)
	{
		WriteBig32(dataPtr + index, BlockPtr.Entries[i]);
		index += 4;
	}

	return index;
}

std::shared_ptr<SuperBlock> ParseDNA(std::shared_ptr<std::vector<char>> DnaPtr)
{
	// Create Block
	auto SuperBlk = std::make_shared<SuperBlock>();
	SuperBlk->dataPtr = DnaPtr;

	// Create C-type pointer for access
	auto fdPtr = DnaPtr->data();

	// Index
	uint64_t index = 0;


	// Make Header
	const Header* HeadBlock = reinterpret_cast<const Header*>(fdPtr);
	index = sizeof(Header);
	SuperBlk->Head = HeadBlock;

	

	// Block1
	std::shared_ptr<Block1> UKBlock1 = std::make_shared<Block1>();

	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock1);
	UKBlock1->UnknownValue1 = ToLittle32(fdPtr + index);
	index += 4;
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock2);
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock3);
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock4);
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock5);
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock6);
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock7);
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock8);
	index = ReadShortEntry(fdPtr, index, UKBlock1->SubBlock9);

	SuperBlk->Block1 = UKBlock1;




	// Block2
	std::shared_ptr<Block2> UKBlock2 = std::make_shared<Block2>();

	// Fill the first 24 bytes
	memcpy(UKBlock2->Unknown1, fdPtr + index, 24);
	index += 24;

	index = ReadShortEntry(fdPtr, index, UKBlock2->SubBlock1);

	memcpy(UKBlock2->Unknown2, fdPtr + index, 356);
	index += 356;

	SuperBlk->Block2 = UKBlock2;




	// Block Three has data
	std::shared_ptr<Block3> UKBlock3 = std::make_shared<Block3>();


	// Read BoneNames
	index = ReadStringEntry(fdPtr, index, UKBlock3->SubBlock1_CTRLNames);
	index = ReadStringEntry(fdPtr, index, UKBlock3->SubBlock2_ExpressionNames);
	index = ReadStringEntry(fdPtr, index, UKBlock3->SubBlock3_FacialJointNames);
	index = ReadStringEntry(fdPtr, index, UKBlock3->SubBlock4_OtherJointNames);
	index = ReadStringEntry(fdPtr, index, UKBlock3->SubBlock5_OtherStrings);
	index = ReadStringEntry(fdPtr, index, UKBlock3->SubBlock6_Meshes);

	index = ReadShortEntry(fdPtr, index, UKBlock3->SubBlock7);
	index = ReadShortEntry(fdPtr, index, UKBlock3->SubBlock8);
	index = ReadShortEntry(fdPtr, index, UKBlock3->SubBlock9);

	index = ReadFloatEntry(fdPtr, index, UKBlock3->SubBlock10);
	index = ReadFloatEntry(fdPtr, index, UKBlock3->SubBlock11);
	index = ReadFloatEntry(fdPtr, index, UKBlock3->SubBlock12);

	index = ReadFloatEntry(fdPtr, index, UKBlock3->SubBlock13_JointOrientationX);
	index = ReadFloatEntry(fdPtr, index, UKBlock3->SubBlock14_JointOrientationY);
	index = ReadFloatEntry(fdPtr, index, UKBlock3->SubBlock15_JointOrientationZ);


	SuperBlk->Block3 = UKBlock3;

	// Block Four has data
	std::shared_ptr<Block4> UKBlock4 = std::make_shared<Block4>();

	UKBlock4->Unknown = ToLittle16(fdPtr + index);
	index += 2;

	index = ReadShortEntry(fdPtr, index, UKBlock4->SubBlock1);
	index = ReadShortEntry(fdPtr, index, UKBlock4->SubBlock2);

	index = ReadFloatEntry(fdPtr, index, UKBlock4->SubBlock3);
	index = ReadFloatEntry(fdPtr, index, UKBlock4->SubBlock4);
	index = ReadFloatEntry(fdPtr, index, UKBlock4->SubBlock5);
	index = ReadFloatEntry(fdPtr, index, UKBlock4->SubBlock6);

	index = ReadShortEntry(fdPtr, index, UKBlock4->SubBlock7);
	index = ReadShortEntry(fdPtr, index, UKBlock4->SubBlock8);

	index = ReadFloatEntry(fdPtr, index, UKBlock4->SubBlock9);

	SuperBlk->Block4 = UKBlock4;

	return SuperBlk;
}

std::shared_ptr<std::vector<char>> SerialiseDna(std::shared_ptr<SuperBlock> BlkPtr)
{
	// SuperBlock contains original data. Use it resize
	std::shared_ptr<std::vector<char>> SerialDna = std::make_shared<std::vector<char>>();
	SerialDna->resize(BlkPtr->dataPtr->size());

	uint64_t index = 0;
	auto serialPtr = SerialDna->data();

	// The header is unknown
	memcpy(serialPtr + index, BlkPtr->Head, sizeof(Header));
	index += sizeof(Header);

	// Block1 is understood, so reverse it
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock1);
	WriteBig32(serialPtr + index, BlkPtr->Block1->UnknownValue1);
	index += 4;
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock2);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock3);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock4);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock5);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock6);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock7);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock8);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block1->SubBlock9);

	// Now Block2
	memcpy(serialPtr + index, BlkPtr->Block2->Unknown1, 24);
	index += 24;
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block2->SubBlock1);
	memcpy(serialPtr + index, BlkPtr->Block2->Unknown2, 356);
	index += 356;

	// Block3
	index = WriteStringEntry(serialPtr, index, BlkPtr->Block3->SubBlock1_CTRLNames);
	index = WriteStringEntry(serialPtr, index, BlkPtr->Block3->SubBlock2_ExpressionNames);
	index = WriteStringEntry(serialPtr, index, BlkPtr->Block3->SubBlock3_FacialJointNames);
	index = WriteStringEntry(serialPtr, index, BlkPtr->Block3->SubBlock4_OtherJointNames);
	index = WriteStringEntry(serialPtr, index, BlkPtr->Block3->SubBlock5_OtherStrings);
	index = WriteStringEntry(serialPtr, index, BlkPtr->Block3->SubBlock6_Meshes);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block3->SubBlock7);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block3->SubBlock8);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block3->SubBlock9);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block3->SubBlock10);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block3->SubBlock11);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block3->SubBlock12);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block3->SubBlock13_JointOrientationX);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block3->SubBlock14_JointOrientationY);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block3->SubBlock15_JointOrientationZ);

	// Block4
	WriteBig16(serialPtr + index, BlkPtr->Block4->Unknown);
	index += 2;
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block4->SubBlock1);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block4->SubBlock2);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block4->SubBlock3);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block4->SubBlock4);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block4->SubBlock5);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block4->SubBlock6);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block4->SubBlock7);
	index = WriteShortEntry(serialPtr, index, BlkPtr->Block4->SubBlock8);
	index = WriteFloatEntry(serialPtr, index, BlkPtr->Block4->SubBlock9);

	// Rest
	memcpy(serialPtr + index, BlkPtr->dataPtr->data() + index, BlkPtr->dataPtr->size() - index);

	return SerialDna;
}

double CalculateFileDiff(std::shared_ptr<std::vector<char>> FirstDNAData, std::shared_ptr<std::vector<char>> SecondDNAData)
{
	auto fdPtr = FirstDNAData->data();
	auto sdPtr = SecondDNAData->data();

	// Okay, now check the sizes
	//std::cout << FirstDNAData.size() << " - " << SecondDNAData.size() << std::endl;

	// Byte Overlay
	auto smallestSize = std::min(FirstDNAData->size(), SecondDNAData->size());

	bool FirstHit = false;

	uint64_t delta = 0;
	for (uint32_t i = 0; i < smallestSize; ++i)
	{
		if (fdPtr[i] == sdPtr[i])
		{
			// Do Nothing
		}
		else
		{
			if (!FirstHit)
			{
				FirstHit = true;
				std::cout << "Difference at " << std::hex << i << std::dec << ". Log:" << std::endl << "\tOriginal: " << std::hex;

				for (uint32_t a = 0; a < 20; ++a)
				{
					std::cout << (unsigned char)(fdPtr[i - 10 + a]);
				}

				std::cout << std::endl << "\tModified: ";

				for (uint32_t a = 0; a < 20; ++a)
				{
					std::cout << (unsigned char)(sdPtr[i - 10 + a]);
				}

				std::cout << std::dec << std::endl;

			}
			delta += 1;

		}
	}

	return (delta / double(smallestSize)) * 100;

	//std::cout << "Files deviate by " << std::dec << (delta / double(smallestSize)) * 100 << std::endl;
}

std::optional<std::shared_ptr<SuperBlock>> ReadDNAFile(std::string path)
{
	std::ifstream DNAFile(path, std::ios::ate | std::ios::binary);

	if (DNAFile)
	{
		auto DNADataOpt = ReadToVector(DNAFile);
		if (DNADataOpt)
		{
			std::shared_ptr<std::vector<char>> DNAData = DNADataOpt.value();

			auto Returnable = ParseDNA(DNAData);

			return Returnable;
		}
	}

	return std::nullopt;
}

void WriteDNAFile(std::string path, std::shared_ptr<SuperBlock> SuperBlock)
{
	std::ofstream OutFile(path, std::ios::binary);

	if (OutFile)
	{
		// SuperBlock holds our data
		auto SuperIo = SerialiseDna(SuperBlock);

		OutFile.write(SuperIo->data(), SuperIo->size());
	}
}

void Tests(std::shared_ptr<SuperBlock> SuperBlock)
{
	// Modified Joint Positions
	for (uint32_t i = 0; i < SuperBlock->Block3->SubBlock10.EntryCount; ++i)
	{
		SuperBlock->Block3->SubBlock10.Entries[i] += 4.1f;
	}

	for (uint32_t i = 0; i < SuperBlock->Block3->SubBlock11.EntryCount; ++i)
	{
		SuperBlock->Block3->SubBlock11.Entries[i] += 3.1f;
	}

	for (uint32_t i = 0; i < SuperBlock->Block3->SubBlock12.EntryCount; ++i)
	{
		SuperBlock->Block3->SubBlock12.Entries[i] += 5.1f;
	}



	//// Modified Joint Positions
	//for (uint32_t i = 0; i < SuperBlock->Block3->SubBlock13_JointOrientationX.EntryCount; ++i)
	//{
	//	SuperBlock->Block3->SubBlock13_JointOrientationX.Entries[i] *= 1.1f;
	//}

	//for (uint32_t i = 0; i < SuperBlock->Block3->SubBlock14_JointOrientationY.EntryCount; ++i)
	//{
	//	SuperBlock->Block3->SubBlock14_JointOrientationY.Entries[i] *= 1.1f;
	//}

	//for (uint32_t i = 0; i < SuperBlock->Block3->SubBlock15_JointOrientationZ.EntryCount; ++i)
	//{
	//	SuperBlock->Block3->SubBlock15_JointOrientationZ.Entries[i] *= 1.1f;
	//}
}

int main(int argc, char** argv)
{
	if (2 == argc)
	{
		auto DnaOpt = ReadDNAFile(argv[1]);

		if (DnaOpt)
		{
			auto SuperBlk = DnaOpt.value();

			auto SuperSerial = SerialiseDna(SuperBlk);

			std::cout << "Files deviate by " << std::dec << CalculateFileDiff(SuperBlk->dataPtr, SuperSerial) << "%" << std::endl;

			// Modified with Tests
			//Tests(SuperBlk);

			WriteDNAFile("test.dna", SuperBlk);


			// TestWriting

			for (uint16_t i = 0; i < SuperBlk->Block3->SubBlock10.EntryCount; ++i)
			{
				uint16_t Little1 = i;

				// Lookup Bone Name
				if (Little1 < SuperBlk->Block3->SubBlock3_FacialJointNames.EntryCount)
				{
					std::cout << SuperBlk->Block3->SubBlock3_FacialJointNames.Entries[Little1].Src << " (" << Little1 << ")";;
				}
				else
				{
					std::cout << "BadIndex_" << Little1;
				}

				std::cout << " -> ";

				// Lookup Bone Name
				if (Little1 < SuperBlk->Block3->SubBlock10.EntryCount)
				{
					std::cout << SuperBlk->Block3->SubBlock10.Entries[Little1] << "\t" <<
						SuperBlk->Block3->SubBlock11.Entries[Little1] << "\t" <<
						SuperBlk->Block3->SubBlock12.Entries[Little1];
				}
				else
				{
					std::cout << "BadIndex_" << Little1;
				}





				std::cout << std::endl;

			}


			//for (uint16_t i = 0; i < SuperBlk->Block1->SubBlock3.EntryCount; ++i)
			//{
			//	uint16_t Little1 = i;
			//	uint16_t Little2 = SuperBlk->Block1->SubBlock3.Entries[i];

			//	// Lookup Bone Name
			//	if (Little1 < SuperBlk->Block3->SubBlock2_ExpressionNames.EntryCount)
			//	{
			//		std::cout << SuperBlk->Block3->SubBlock2_ExpressionNames.Entries[Little1].Src << " (" << Little1 << ")";
			//	}
			//	else
			//	{
			//		std::cout << "BadIndex_" << Little1;
			//	}

			//	std::cout << " -> ";

			//	// Lookup Bone Name
			//	if (Little2 < SuperBlk->Block3->SubBlock3_FacialJointNames.EntryCount)
			//	{
			//		std::cout << SuperBlk->Block3->SubBlock3_FacialJointNames.Entries[Little2].Src << " (" << Little2 << ")";;
			//	}
			//	else
			//	{
			//		std::cout << "BadIndex_" << Little2;
			//	}

			//	std::cout << std::endl;

			//}
		}
		else
		{
			std::cerr << "File Failed to Read" << std::endl;
		}
	}
	else if (argc > 2)
	{
		auto DnaOpt = ReadDNAFile(argv[1]);
		auto Import = LoadFileFromPath(argv[2]);

		if (DnaOpt && Import->Scene)
		{
			auto SuperBlk = DnaOpt.value();

			auto SuperSerial = SerialiseDna(SuperBlk);

			auto FBXBones = GetBones(Import);


			for (uint16_t i = 0; i < SuperBlk->Block3->SubBlock10.EntryCount; ++i)
			{
				uint16_t Little1 = i;
				std::string cBoneName;

				// Lookup Bone Name
				if (Little1 < SuperBlk->Block3->SubBlock3_FacialJointNames.EntryCount)
				{
					std::cout << SuperBlk->Block3->SubBlock3_FacialJointNames.Entries[Little1].Src << " (" << Little1 << ")";
					cBoneName = std::string(SuperBlk->Block3->SubBlock3_FacialJointNames.Entries[Little1].Src);
				}
				else
				{
					std::cout << "BadIndex_" << Little1;
				}

				std::cout << " -> " << std::endl;

				// Lookup Bone Name Positions
				if (Little1 < SuperBlk->Block3->SubBlock10.EntryCount && FBXBones->contains(cBoneName))
				{
					if (CompareFloat(SuperBlk->Block3->SubBlock10.Entries[Little1], (*FBXBones)[cBoneName].BonePosition.x))
					{
						std::cout << "\t\tX " << SuperBlk->Block3->SubBlock10.Entries[Little1] << " -> " << (*FBXBones)[cBoneName].BonePosition.x << std::endl;
						SuperBlk->Block3->SubBlock10.Entries[Little1] = (*FBXBones)[cBoneName].BonePosition.x;
					}

					if (CompareFloat(SuperBlk->Block3->SubBlock11.Entries[Little1],(*FBXBones)[cBoneName].BonePosition.y))
					{
						std::cout << "\t\tY " << SuperBlk->Block3->SubBlock11.Entries[Little1] << " -> " << (*FBXBones)[cBoneName].BonePosition.y << std::endl;
						SuperBlk->Block3->SubBlock11.Entries[Little1] = (*FBXBones)[cBoneName].BonePosition.y;
					}

					if (CompareFloat(SuperBlk->Block3->SubBlock12.Entries[Little1], (*FBXBones)[cBoneName].BonePosition.z))
					{
						std::cout << "\t\tZ " << SuperBlk->Block3->SubBlock12.Entries[Little1] << " -> " << (*FBXBones)[cBoneName].BonePosition.z << std::endl;
						SuperBlk->Block3->SubBlock12.Entries[Little1] = (*FBXBones)[cBoneName].BonePosition.z;
					}

					if (CompareFloat(SuperBlk->Block3->SubBlock13_JointOrientationX.Entries[Little1], (*FBXBones)[cBoneName].BoneOrientation.x))
					{
						std::cout << "\t\tRX " << SuperBlk->Block3->SubBlock13_JointOrientationX.Entries[Little1] << " -> " << (*FBXBones)[cBoneName].BoneOrientation.x << std::endl;
					}

					if (CompareFloat(SuperBlk->Block3->SubBlock14_JointOrientationY.Entries[Little1], (*FBXBones)[cBoneName].BoneOrientation.y))
					{
						std::cout << "\t\tRY " << SuperBlk->Block3->SubBlock14_JointOrientationY.Entries[Little1] << " -> " << (*FBXBones)[cBoneName].BoneOrientation.y << std::endl;
					}

					if (CompareFloat(SuperBlk->Block3->SubBlock15_JointOrientationZ.Entries[Little1], (*FBXBones)[cBoneName].BoneOrientation.z))
					{
						std::cout << "\t\tRZ " << SuperBlk->Block3->SubBlock15_JointOrientationZ.Entries[Little1] << " -> " << (*FBXBones)[cBoneName].BoneOrientation.z << std::endl;
					}
				}
				else
				{
					std::cout << "BadIndex_" << Little1;
				}
				std::cout << std::endl;

			}

			
			
			WriteDNAFile("test.dna", SuperBlk);
		}	
	}
	
	return 0;
}