//
// Created by ceduardosq on 10/14/21.
//

#ifndef FILEORGANIZATION_EXPANDIBLEHASHING_H
#define FILEORGANIZATION_EXPANDIBLEHASHING_H

#include "../libs.h"
#include "Bucket.h"

template<typename K, typename R>
class ExpandibleHashing {
    filename _indexFile{};
    filename _dataFile{};
    filename _propertyFile{};
    depth _globalDepth{};

    position toPosition(K key) {
        position k = position(k);
        return k;
    }

    position hash(position k) {
        return k & ((1LL << _globalDepth) - 1);
    }

    position searchIndex(position key) {
        position dataPos = hash(key) * sizeof(position);
        std::fstream indexFile(_indexFile, std::ios::binary);
        if (!indexFile.is_open()) throw std::invalid_argument("Cannot open index file.");
        indexFile.seekg(dataPos);
        position bucketPos{};
        indexFile.read((char *) &bucketPos, sizeof(bucketPos));
        indexFile.close();
        return bucketPos;
    }

    position searchBucket(position bucketPos) {
        std::fstream dataFile(_dataFile, std::ios::binary);
        if (!dataFile.is_open()) throw std::invalid_argument("Cannot open data file.");
        while (bucketPos != -1) {
            Bucket<R> bucket{};
            dataFile.seekg(bucketPos);
            dataFile.read((char *) &bucket, sizeof(bucket));
            if (bucket._size < M || bucket._next == -1) break;
            bucketPos = bucket._next;
        }
        dataFile.close();
        return bucketPos;
    }

    void splitBucket(position bucket, position bits, depth localDepth) {
        std::fstream indexFile(_indexFile, std::ios::binary);
        if (!indexFile.is_open()) throw std::invalid_argument("Cannot open index file.");
        position n = 1LL << (_globalDepth - localDepth);
        for (position i = 0; i < n; ++i) {
            position j = (bits | (i << localDepth));
            indexFile.seekg(j * sizeof(position));
            indexFile.write((char *) &bucket, sizeof(bucket));
        }
        indexFile.close();
    }

    void _insert(position key, R Record) {
        position bucketPos = searchBucket(searchIndex(key));
        std::fstream dataFile(_dataFile, std::ios::binary);
        if (!dataFile.is_open()) throw std::invalid_argument("Cannot open data file.");
        dataFile.seekg(bucketPos);
        Bucket<R> bucket{};
        dataFile.read((char *) &bucket, sizeof(bucket));
        if (bucket._size < M) {
            bucket._values[bucket._size++] = std::pair<position, R>(toPosition(key), Record);
            dataFile.seekg(bucketPos);
            dataFile.write((char *) &bucket, sizeof(bucket));
        } else if (bucket._size == M) {
            if (bucket._localDepth == M) {
                dataFile.seekg(0, std::ios::end);
                position newBucketPos = dataFile.tellg();
                Bucket<R> newBucket(bucket._localDepth, bucket._bits);
                dataFile.write((char *) &newBucket, sizeof(newBucket));
                bucket._next = newBucketPos;
                dataFile.seekg(bucketPos);
                dataFile.write((char *) &bucket, sizeof(bucket));
            } else {
                Bucket<R> newBucket(bucket._localDepth + 1, bucket._bits | (1LL << bucket._localDepth));
                bucket._localDepth++;
                std::vector<std::pair<position, R>> values = {std::pair<position, R>(key, Record)};
                for (int i = 0; i < bucket._size; ++i)
                    values.push_back(bucket._values[i]);
                bucket._size = 0;

                dataFile.seekg(bucketPos);
                dataFile.write((char *) &bucket, sizeof(bucket));
                dataFile.seekg(0, std::ios::end);
                position newBucketPos = dataFile.tellg();
                dataFile.write((char *) &newBucket, sizeof(bucket));
                splitBucket(newBucketPos, newBucket._bits, newBucket._localDepth);
                dataFile.close();
                for (const auto &e: values)
                    insert(e.first, e.second);
            }
        }
        dataFile.close();
    }

public:
    explicit ExpandibleHashing(const filename &name) : _indexFile(name + "index.bin"), _dataFile(name + "data.bin"),
                                                       _propertyFile(name + "property.bin") {
        std::fstream propertyFile(_propertyFile, std::ios::binary);
        if (!propertyFile.is_open()) throw std::invalid_argument("Cannot open property file.");
        propertyFile.read((char *) &_globalDepth, sizeof(depth));
        propertyFile.close();
    }

    explicit ExpandibleHashing(const filename &name, depth globalDepth) : _globalDepth(globalDepth),
                                                                          _indexFile(name + "-index.bin"),
                                                                          _dataFile(name + "-data.bin"),
                                                                          _propertyFile(name + "-property.bin") {
        std::ofstream indexFile(_indexFile, std::ios::binary | std::ios::trunc);
        if (!indexFile.is_open()) throw std::invalid_argument("Cannot open index file.");
        int n = (1 << globalDepth);
        for (int i = 0; i < n; ++i) {
            position content = i % 2 * sizeof(Bucket<R>);
            indexFile.write((char *) &content, sizeof(position));
        }
        indexFile.close();

        std::ofstream dataFile(_dataFile, std::ios::binary | std::ios::trunc);
        if (!dataFile.is_open()) throw std::invalid_argument("Cannot open data file.");
        Bucket<R> bucket0(1, 0), bucket1(1, 1);
        dataFile.write((char *) &bucket0, sizeof(bucket0));
        dataFile.write((char *) &bucket1, sizeof(bucket0));
        dataFile.close();

        std::ofstream propertyFile(_propertyFile, std::ios::binary | std::ios::trunc);
        if (!propertyFile.is_open()) throw std::invalid_argument("Cannot open property file.");
        propertyFile.write((char *) &_globalDepth, sizeof(globalDepth));
        propertyFile.close();
    }


    std::pair<answer, R> search(K key) {
        position bucketPos = searchIndex(key);
        while (bucketPos != -1) {
            Bucket<R> bucket{};
            std::fstream dataFile(_dataFile, std::ios::binary);
            dataFile.seekg(bucketPos);
            dataFile.read((char *) &bucket, sizeof(bucket));
            dataFile.close();
            auto result = bucket.search(toPosition(key));
            if (result.first == true) return result;
            bucketPos = bucket._next;
        }
        return std::pair<answer, R>(false, R{});
    }

    std::vector<R> rangeSearch(K keyA, K keyB) {
        if (keyA > keyB) throw std::invalid_argument("Second key must be greater or equal to the first one.");
        std::vector<R> result{};
        for (K k = keyA; k <= keyB; ++k) {
            auto e = search(k);
            if (e.first == true) result.push_back(e.second);
        }
        return result;
    }

    void insert(K key, R Record) {
        _insert(toPosition(key), Record);
    }

    void remove(K key) {
        position bucketPos = searchIndex(key);
        while (bucketPos != -1) {
            Bucket<R> bucket{};
            std::fstream dataFile(_dataFile, std::ios::binary);
            dataFile.seekg(bucketPos);
            dataFile.read((char *) &bucket, sizeof(bucket));
            dataFile.close();
            auto result = bucket.search(toPosition(key));
            if (result.first == true) {
                bucket.remove(toPosition(key));
                break;
            }
            bucketPos = bucket._next;
        }
    }
};

#endif //FILEORGANIZATION_EXPANDIBLEHASHING_H
