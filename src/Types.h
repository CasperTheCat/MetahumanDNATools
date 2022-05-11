#pragma once
#include <cstdint>

inline uint32_t ToLittle32(const void* pValue)
{
	auto dataPtr = reinterpret_cast<const unsigned char*>(pValue);
	uint32_t outValue = (dataPtr[3] << 0) | (dataPtr[2] << 8) | (dataPtr[1] << 16) | (dataPtr[0] << 24);
	return outValue;
}

inline uint16_t ToLittle16(const void* pValue)
{
	auto dataPtr = reinterpret_cast<const unsigned char*>(pValue);
	uint16_t outValue = (dataPtr[1] << 0) | (dataPtr[0] << 8);
	return outValue;
}

void WriteBig32(void* Dst, uint32_t Value)
{
	auto dataPtr = reinterpret_cast<uint32_t*>(Dst);
	*dataPtr = ToLittle32(&Value);
}

void WriteBig32(void* Dst, float Value)
{
	uint32_t ValueAsU32 = *(reinterpret_cast<uint32_t*>(&Value));
	auto dataPtr = reinterpret_cast<uint32_t*>(Dst);
	*dataPtr = ToLittle32(&Value);
}

void WriteBig16(void* Dst, uint16_t Value)
{
	auto dataPtr = reinterpret_cast<uint16_t*>(Dst);
	*dataPtr = ToLittle16(&Value);
}



struct StringEntry
{
	uint32_t StrLen;
	const char* Src;
	//char String[1];
};

struct StringEntries
{
	uint32_t EntryCount;
	std::vector<StringEntry> Entries;
};

struct ShortEntries
{
	uint32_t EntryCount;
	std::vector<uint16_t> Entries;
};

struct FloatEntries
{
	uint32_t EntryCount;
	std::vector<float> Entries;
};

struct Header
{
	char Unknown[0x6B];
};

struct Block1
{
	ShortEntries SubBlock1; // Really Short
	uint32_t UnknownValue1;
	
	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock2; // Matches size closest with FacialJointNames

	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock3; // Matches size closest with OtherJointNames

	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock4; // Matches size closest with Something?

	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock5; // Too big for inputs?

	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock6; // Too big for OtherStrings?

	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock7; // 

	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock8;

	// Contains values up to 887 (or FacialJointNames)
	ShortEntries SubBlock9;
};

struct Block2
{
	char Unknown1[24];
	ShortEntries SubBlock1; // Same size as OtherJointNames. Values are the indices?
	char Unknown2[356];
};

struct Block3
{
	StringEntries SubBlock1_CTRLNames;

	// Listed as inputs in Maya rl4Embedded
	StringEntries SubBlock2_ExpressionNames; 

	// Named in Maya, but not listed in rl4
	StringEntries SubBlock3_FacialJointNames;

	// Listed as Bs outputs in Maya
	StringEntries SubBlock4_CorrectiveBlendShapeNames;

	// Listed as Am outputs in Maya 
	StringEntries SubBlock5_OtherStrings;

	// List of Maya meshes
	StringEntries SubBlock6_Meshes;

	ShortEntries SubBlock7;
	ShortEntries SubBlock8;
	ShortEntries SubBlock9;

	FloatEntries SubBlock10; // X
	FloatEntries SubBlock11; // Y
	FloatEntries SubBlock12; // Z

	FloatEntries SubBlock13_JointOrientationX; // X
	FloatEntries SubBlock14_JointOrientationY; // Y
	FloatEntries SubBlock15_JointOrientationZ; // Z

};

struct Block4
{
	uint16_t Unknown;
	ShortEntries SubBlock1;
	ShortEntries SubBlock2;

	FloatEntries SubBlock3;
	FloatEntries SubBlock4;
	FloatEntries SubBlock5; 
	FloatEntries SubBlock6;

	ShortEntries SubBlock7;
	ShortEntries SubBlock8;

	FloatEntries SubBlock9;
};


struct SuperBlock
{
	const Header* Head;
	std::shared_ptr<Block1> Block1;
	std::shared_ptr<Block2> Block2;
	std::shared_ptr<Block3> Block3;
	std::shared_ptr<Block4> Block4;
	
	std::shared_ptr<std::vector<char>> dataPtr;
	uint64_t dbgIndex;
};
