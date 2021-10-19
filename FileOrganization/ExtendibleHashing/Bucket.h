//
// Created by ceduardosq on 10/15/21.
//

#ifndef FILEORGANIZATION_BUCKET_H
#define FILEORGANIZATION_BUCKET_H

#include "const.h"

template<typename R>
struct Bucket {
    depth _localDepth{};
    position _bits{};
    size _size{};
    std::pair<position, R> _values[M]{};
    position _next{};
public:
    Bucket() = default;

    explicit Bucket(depth localDepth, position bits) : _localDepth(localDepth), _bits(bits), _size(0), _next(-1) {}

    std::pair<answer, R> search(position key) {
        for (position i = 0; i < _size; ++i)
            if (_values[i].first == key) return std::pair<answer, R>(true, _values[i].second);
        return std::pair<answer, R>(false, R{});
    }

    void remove(position key) {
        std::vector<std::pair<position, R>> values;
        for (const auto &e: _values) values.push_back(e);
        for (auto it = values.begin(); it < values.end(); ++it) {
            if (it->first == key) {
                values.erase(it);
                break;
            }
        }
        _size = values.size();
        for (position i = 0; i < _size; ++i) _values[i] = values[i];
    }
};

#endif //FILEORGANIZATION_BUCKET_H
