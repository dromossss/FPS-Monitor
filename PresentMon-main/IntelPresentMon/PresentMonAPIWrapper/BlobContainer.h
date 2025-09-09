#pragma once
#include <memory>
#include <vector>
#include <cstdint>

namespace pmapi
{
    // BlobContainer makes dealing with query data blobs easier and safer by:
    // managing an array of blobs, sized properly by constructing containers from the queries themselves
    // managing in-out function parameters for available size and populated blob count
    // providing convenient iterators to iterate over blob data (using range-based for loops etc.)
    // typically, should not be contructed directly, but rather from a DynamicQuery or FrameQuery
    class BlobContainer
    {
        friend class FrameQuery;
        friend class DynamicQuery;
    public:
        // create empty blob container
        BlobContainer() = default;
        // create blob container for a specific query size nBlobs x blobSize bytes
        BlobContainer(const void* handle, size_t blobSize, uint32_t nBlobs);
        // move ctor
        BlobContainer(BlobContainer&& other) noexcept;
        // copy ctor
        BlobContainer(const BlobContainer& other);
        // move assign
        BlobContainer& operator=(BlobContainer&& rhs) noexcept;
        // copy assign
        BlobContainer& operator=(const BlobContainer& rhs);
        // size of all blobs in bytes
        size_t GetTotalSize() const;
        // size of a single blob in bytes
        size_t GetBlobSize() const;
        // get number of blobs in container (controls # of swap chains polled for DynamicQuery, # of frames consumed for FrameQuery)
        uint32_t GetBlobCount() const;
        // get the number of blobs that were actually populated the last time this container was used in query call
        uint32_t GetNumBlobsPopulated() const;
        // get a pointer to the first byte of the first blob
        uint8_t* GetFirst();
        // get a pointer to the first byte of the first blob (immutable)
        const uint8_t* GetFirst() const;
        // get pointer to the first byte of the nth blob
        uint8_t* operator[](size_t index);
        // get pointer to the first byte of the nth blob (immutable)
        const uint8_t* operator[](size_t index) const;
        // beginning of populated range of blobs
        std::vector<uint8_t*>::const_iterator begin() const;
        // end of populated range of blobs
        std::vector<uint8_t*>::const_iterator end() const;
        // check if all blobs were populated, useful when consuming frames to see if there might be frames left in the queue yet
        bool AllBlobsPopulated() const;
        // check if this container is associated with query having handle
        template<typename T>
        bool CheckHandle(T handle) const
        {
            return static_cast<const void*>(handle) == handle_;
        }
        // clear out this container (free owned resources)
        void Reset();
        // check if empty
        bool Empty() const;
        // aliases Empty()
        operator bool() const;
    private:
        // functions
        void PopulateBlobPointers_();
        uint32_t& AcquireNumBlobsInRef_();
        // data
        const void* handle_ = nullptr;
        size_t blobSize_ = 0;
        uint32_t nBlobs_ = 0;
        uint32_t nBlobsFilledInOut_ = 0;
        std::unique_ptr<uint8_t[]> pBlobArrayBytes_;
        std::vector<uint8_t*> blobPointers_;
    };
}