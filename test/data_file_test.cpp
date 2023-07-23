#include <gtest/gtest.h>

#include "data/common.hpp"
#include "data/iterators.hpp"

#include <string_view>
#include <vector>

#include "utils/paths.h"

namespace devilution {
auto LoadDataFile(std::string_view file)
{
	std::string unitTestFolderCompletePath = paths::BasePath() + "/test/fixtures/";
	paths::SetAssetsPath(unitTestFolderCompletePath);
	return DataFile::load(file);
}

void TestFileContents(
    const DataFile &dataFile,
    std::vector<std::vector<std::string_view>> expectedContent,
    GetFieldResult::Status expectedEndOfRecordStatus = GetFieldResult::Status::EndOfRecord,
    GetFieldResult::Status expectedEndOfFileStatus = GetFieldResult::Status::EndOfFile)
{
	GetFieldResult result { dataFile.begin() };
	unsigned row = 0;
	do {
		ASSERT_LT(row, expectedContent.size()) << "Too many records";
		unsigned col = 0;
		do {
			ASSERT_LT(col, expectedContent[row].size()) << "Too many fields in record " << row;
			result = GetNextField(result.next, dataFile.end());
			EXPECT_EQ(result.value, expectedContent[row][col]) << "Unexpected value at record " << row << " and field " << col;
			col++;
		} while (!result.endOfRecord());
		if (!result.endOfFile())
			EXPECT_EQ(result.status, expectedEndOfRecordStatus) << "Unexpected status when parsing the end of record " << row;

		EXPECT_EQ(col, expectedContent[row].size()) << "Parsing returned fewer fields than expected in record " << row;
		row++;
	} while (!result.endOfFile());
	EXPECT_EQ(result.status, expectedEndOfFileStatus) << "Unexpected status when parsing the end of the file";

	EXPECT_EQ(row, expectedContent.size()) << "Parsing returned fewer records than expected";
}

TEST(DataFileTest, TryLoadMissingFile)
{
	auto result = LoadDataFile("txtdata\\not_found.tsv");
	EXPECT_FALSE(result.has_value()) << "Trying to load a non-existent file should return an unexpected/error response";

	EXPECT_EQ(result.error(), DataFile::Error::NotFound) << "The error code should indicate the file was not found";
}

TEST(DataFileTest, LoadCRFile)
{
	auto result = LoadDataFile("txtdata\\cr.tsv");
	ASSERT_TRUE(result.has_value()) << "Unable to load cr.tsv";

	DataFile &dataFile = result.value();
	EXPECT_EQ(dataFile.size(), 33) << "File size should be reported in code units";

	std::vector<std::vector<std::string_view>> expectedFields {
		{ "Test", "Empty", "Values" },
		{ "", "2", "3" },
		{ "1", "2", "" },
		{ "1", "", "3" }
		// because the file does not end with a newline parsing stops at the previous record
	};

	TestFileContents(dataFile, expectedFields, GetFieldResult::Status::BadRecordSeparator, GetFieldResult::Status::FileTruncated);
}

TEST(DataFileTest, LoadWindowsFile)
{
	auto result = LoadDataFile("txtdata\\crlf.tsv");
	ASSERT_TRUE(result.has_value()) << "Unable to load crlf.tsv";

	DataFile &dataFile = result.value();
	EXPECT_EQ(dataFile.size(), 37) << "File size should be reported in code units";

	std::vector<std::vector<std::string_view>> expectedFields {
		{ "Test", "Empty", "Values" },
		{ "", "2", "3" },
		{ "1", "2", "" },
		{ "1", "", "3" },
		{ "" } // file ends with a newline, parsing returns a single empty field
	};

	TestFileContents(dataFile, expectedFields);
}

TEST(DataFileTest, LoadTypicalFile)
{
	auto result = LoadDataFile("txtdata\\lf.tsv");
	ASSERT_TRUE(result.has_value()) << "Unable to load lf.tsv";

	DataFile &dataFile = result.value();
	EXPECT_EQ(dataFile.size(), 33) << "File size should be reported in code units";

	std::vector<std::vector<std::string_view>> expectedFields {
		{ "Test", "Empty", "Values" },
		{ "", "2", "3" },
		{ "1", "2", "" },
		{ "1", "", "3" },
		{ "" } // file ends with a newline, parsing returns a single empty field
	};

	TestFileContents(dataFile, expectedFields);
}

TEST(DataFileTest, LoadUtf8WithBOM)
{
	auto result = LoadDataFile("txtdata\\utf8_bom.tsv");
	ASSERT_TRUE(result.has_value()) << "Unable to load utf8_bom.tsv";

	DataFile &dataFile = result.value();
	EXPECT_EQ(dataFile.size(), 33) << "Loading a file containing a UTF8 byte order marker should strip that prefix";

	std::vector<std::vector<std::string_view>> expectedFields {
		{ "Test", "Empty", "Values" },
		{ "", "2", "3" },
		{ "1", "2", "" },
		{ "1", "", "3" },
		{ "" } // file ends with a newline, parsing returns a single empty field
	};

	TestFileContents(dataFile, expectedFields);
}

TEST(DataFileTest, IterateOverRecords)
{
	auto result = LoadDataFile("txtdata\\lf.tsv");
	ASSERT_TRUE(result.has_value()) << "Unable to load lf.tsv";

	DataFile &dataFile = result.value();

	std::vector<std::vector<std::string_view>> expectedFields {
		{ "Test", "Empty", "Values" },
		{ "", "2", "3" },
		{ "1", "2", "" },
		{ "1", "", "3" },
		{ "" } // file ends with a newline, parsing returns a single empty field
	};

	unsigned row = 0;
	for (std::string_view record : RecordsRange { dataFile.view() }) {
		ASSERT_LT(row, expectedFields.size()) << "Too many records";
		unsigned col = 0;
		for (std::string_view field : FieldsInRecordRange { record }) {
			if (col < expectedFields[row].size())
				EXPECT_EQ(field, expectedFields[row][col]) << "Unexpected value at record " << row << " and field " << col;
			else
				ADD_FAILURE() << "Extra value '" << field << "' in record " << row << " at field " << col;
			col++;
		}
		EXPECT_GE(col, expectedFields[row].size()) << "Parsing returned fewer fields than expected in record " << row;
		row++;
	}
	EXPECT_EQ(row, expectedFields.size()) << "Parsing returned fewer records than expected";
}

TEST(DataFileTest, DiscardAllAfterFirstField)
{
	auto loadDataResult = LoadDataFile("txtdata\\lf.tsv");
	ASSERT_TRUE(loadDataResult.has_value()) << "Unable to load lf.tsv";

	DataFile &dataFile = loadDataResult.value();

	std::array<std::string_view, 5> expectedFields { "Test", "", "1", "1", "" };

	GetFieldResult result { dataFile.begin() };
	unsigned row = 0;
	do {
		ASSERT_LT(row, expectedFields.size()) << "Too many records";
		result = GetNextField(result.next, dataFile.end());
		EXPECT_EQ(result.value, expectedFields[row]) << "Unexpected first value at record " << row;

		if (result.endOfRecord() && !result.endOfFile())
			ADD_FAILURE() << "Parsing returned fewer fields than expected in record " << row;
		else
			result = DiscardRemainingFields(result.next, dataFile.end());
		row++;
	} while (!result.endOfFile());

	EXPECT_EQ(row, expectedFields.size()) << "Parsing returned fewer records than expected";
}

TEST(DataFileTest, DiscardAllAfterFirstFieldIterator)
{
	auto result = LoadDataFile("txtdata\\lf.tsv");
	ASSERT_TRUE(result.has_value()) << "Unable to load lf.tsv";

	DataFile &dataFile = result.value();

	std::array<std::string_view, 5> expectedFields { "Test", "", "1", "1", "" };

	unsigned row = 0;
	for (std::string_view record : RecordsRange { dataFile.view() }) {
		ASSERT_LT(row, expectedFields.size()) << "Too many records";
		for (std::string_view field : FieldsInRecordRange { record }) {
			EXPECT_EQ(field, expectedFields[row]) << "Unexpected first value at record " << row;
			break;
		}
		row++;
	}
}

TEST(DataFileTest, DiscardAllUpToLastField)
{
	auto loadDataResult = LoadDataFile("txtdata\\lf.tsv");
	ASSERT_TRUE(loadDataResult.has_value()) << "Unable to load lf.tsv";

	DataFile &dataFile = loadDataResult.value();

	std::array<std::string_view, 5> expectedFields { "Values", "3", "", "3", "" };

	GetFieldResult result { dataFile.begin() };
	unsigned row = 0;
	do {
		ASSERT_LT(row, expectedFields.size()) << "Too many records";
		result = DiscardMultipleFields(result.next, dataFile.end(), 2);
		if (row < expectedFields.size() - 1) {
			EXPECT_FALSE(result.endOfRecord()) << "Parsing returned fewer fields than expected in record " << row;
			if (!result.endOfRecord())
				result = GetNextField(result.next, dataFile.end());
		}
		EXPECT_EQ(result.value, expectedFields[row]) << "Unexpected last value at record " << row;

		if (!result.endOfRecord()) {
			ADD_FAILURE() << "Parsing returned fewer fields than expected in record " << row;
			result = DiscardRemainingFields(result.next, dataFile.end());
		}
		row++;
	} while (!result.endOfFile());

	EXPECT_EQ(row, expectedFields.size()) << "Parsing returned fewer records than expected";
}

} // namespace devilution
