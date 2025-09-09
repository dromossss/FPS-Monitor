#include <gtest/gtest.h>
#include <Windows.h>
#include <bcrypt.h>

#include "../PresentMonUtils/MemBuffer.h"

class MemBufferULT : public ::testing::Test {
 public:
  void SetUp() override { mMemBuffer = std::make_unique<MemBuffer>(); }
  std::unique_ptr<MemBuffer> mMemBuffer;
};

TEST_F(MemBufferULT, MemBufferInitialization) {
	EXPECT_TRUE((mMemBuffer->GetCurrentSize() == 0));
	EXPECT_TRUE((mMemBuffer->AccessMem() == nullptr));
}

TEST_F(MemBufferULT, MemBufferSingleItemAdd) {
	double testData[2];
	testData[0] = 60.0f;
	testData[1] = 120.34f;
	size_t testDataSize = sizeof(testData);
	mMemBuffer->AddItem(testData, testDataSize);
	EXPECT_TRUE((mMemBuffer->GetCurrentSize() == testDataSize));
	
	double* tempTestData = (double*)mMemBuffer->AccessMem();
	EXPECT_TRUE(tempTestData != nullptr);
	EXPECT_TRUE(*tempTestData++ == 60.0f);
	EXPECT_TRUE(*tempTestData++ == 120.34f);
}

TEST_F(MemBufferULT, MemBufferMultiItemAddOne) {
	
	uint32_t testData1 = 0x1645;
	size_t testData1Size = sizeof(testData1);
	uint64_t testData2 = 0xba5eball;
	size_t testData2Size = sizeof(testData2);
	double testData3[2];
	testData3[0] = 60.0f;
	testData3[1] = 120.34f;
	size_t testData3Size = sizeof(testData3);

	mMemBuffer->AddItem(&testData1, testData1Size);
	mMemBuffer->AddItem(&testData2, testData2Size);
	mMemBuffer->AddItem(&testData3, testData3Size);
	EXPECT_TRUE((mMemBuffer->GetCurrentSize() == testData1Size + testData2Size + testData3Size));

	LPVOID testData = mMemBuffer->AccessMem();
	EXPECT_TRUE(testData != nullptr);

	uint32_t* dataItem1 = (uint32_t*)testData;
	EXPECT_TRUE(*dataItem1 == testData1);

	uint64_t* dataItem2 = (uint64_t*)((BYTE*)testData + testData1Size);
	EXPECT_TRUE(*dataItem2 == testData2);

	double* dataItem3 = (double*)((BYTE*)testData + (testData1Size + testData2Size));
	EXPECT_TRUE(*dataItem3++ == 60.0f);
	EXPECT_TRUE(*dataItem3++ == 120.34f);
}

TEST_F(MemBufferULT, MemBufferMultiItemAddTwo) {

	uint32_t testData1 = 0x1645;
	size_t testData1Size = sizeof(testData1);
	uint64_t testData2 = 0xba5eball;
	size_t testData2Size = sizeof(testData2);
	double testData3[2];
	testData3[0] = 60.0f;
	testData3[1] = 120.34f;
	size_t testData3Size = sizeof(testData3);
	uint64_t testData4[10];
	size_t testData4Size = sizeof(testData4);

	BCRYPT_ALG_HANDLE crypto_algo;
    auto status = BCryptOpenAlgorithmProvider(
        &crypto_algo, BCRYPT_RNG_ALGORITHM, NULL, 0);
    EXPECT_TRUE(status >= 0);

	for (uint32_t i = 0; i < 10; i++) {
      ULONG test_data_size = sizeof(uint64_t);
      auto rand_gen_success =
          BCryptGenRandom(crypto_algo, (BYTE*)&testData4[i], test_data_size, 0);
      EXPECT_TRUE(rand_gen_success >= 0);
    }
    status = BCryptCloseAlgorithmProvider(crypto_algo, 0);
    EXPECT_TRUE(status >= 0);

	mMemBuffer->AddItem(&testData1, testData1Size);
	mMemBuffer->AddItem(&testData2, testData2Size);
	mMemBuffer->AddItem(&testData3, testData3Size);
	mMemBuffer->AddItem(&testData4, testData4Size);
	EXPECT_TRUE((mMemBuffer->GetCurrentSize() == testData1Size + testData2Size + testData3Size + testData4Size));

	LPVOID testData = mMemBuffer->AccessMem();
	EXPECT_TRUE(testData != nullptr);

	uint32_t* dataItem1 = (uint32_t*)testData;
	EXPECT_TRUE(*dataItem1 == testData1);

	uint64_t* dataItem2 = (uint64_t*)((BYTE*)testData + testData1Size);
	EXPECT_TRUE(*dataItem2 == testData2);

	double* dataItem3 = (double*)((BYTE*)testData + (testData1Size + testData2Size));
	EXPECT_TRUE(*dataItem3++ == testData3[0]);
	EXPECT_TRUE(*dataItem3++ == testData3[1]);

	uint64_t* dataItem4 = (uint64_t*)((BYTE*)testData + (testData1Size + testData2Size + testData3Size));
	for (uint32_t i = 0; i < 10; i++) {
		EXPECT_TRUE(*dataItem4++ == testData4[i]);
	}
}

TEST_F(MemBufferULT, MemBufferAddAfterAccess) {

	uint32_t testData1 = 0x1645;
	size_t testData1Size = sizeof(testData1);
	uint64_t testData2 = 0xba5eball;
	size_t testData2Size = sizeof(testData2);
	double testData3[2];
	testData3[0] = 60.0f;
	testData3[1] = 120.34f;
	size_t testData3Size = sizeof(testData3);

	mMemBuffer->AddItem(&testData1, testData1Size);
	mMemBuffer->AddItem(&testData2, testData2Size);
	mMemBuffer->AddItem(&testData3, testData3Size);
	EXPECT_TRUE((mMemBuffer->GetCurrentSize() == testData1Size + testData2Size + testData3Size));

	LPVOID testData = mMemBuffer->AccessMem();
	EXPECT_TRUE(testData != nullptr);

	uint32_t* dataItem1 = (uint32_t*)testData;
	EXPECT_TRUE(*dataItem1 == testData1);

	uint64_t* dataItem2 = (uint64_t*)((BYTE*)testData + testData1Size);
	EXPECT_TRUE(*dataItem2 == testData2);

	double* dataItem3 = (double*)((BYTE*)testData + (testData1Size + testData2Size));
	EXPECT_TRUE(*dataItem3++ == 60.0f);
	EXPECT_TRUE(*dataItem3++ == 120.34f);

	uint64_t testData4[10];
	size_t testData4Size = sizeof(testData4);

	BCRYPT_ALG_HANDLE crypto_algo;
    auto status = BCryptOpenAlgorithmProvider(
            &crypto_algo, BCRYPT_RNG_ALGORITHM, NULL, 0);
    EXPECT_TRUE(status >= 0);

	for (uint32_t i = 0; i < 10; i++) {
      ULONG test_data_size = sizeof(uint64_t);
      auto rand_gen_success =
          BCryptGenRandom(crypto_algo, (BYTE*)&testData4[i], test_data_size, 0);
      EXPECT_TRUE(rand_gen_success >= 0);
	}
    status = BCryptCloseAlgorithmProvider(crypto_algo, 0);
    EXPECT_TRUE(status >= 0);

	mMemBuffer->AddItem(&testData4, testData4Size);

	testData = mMemBuffer->AccessMem();

	EXPECT_TRUE((mMemBuffer->GetCurrentSize() == testData1Size + testData2Size + testData3Size + testData4Size));
	uint64_t* dataItem4 = (uint64_t*)((BYTE*)testData + (testData1Size + testData2Size + testData3Size));
	for (uint32_t i = 0; i < 10; i++) {
		EXPECT_TRUE(*dataItem4++ == testData4[i]);
	}
}

TEST_F(MemBufferULT, MemBufferNullInputBuffer) {
        bool status = mMemBuffer->AddItem(nullptr, 0);
        EXPECT_TRUE(status == false);
}