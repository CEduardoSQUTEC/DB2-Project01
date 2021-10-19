//
// Created by ceduardosq on 10/14/21.
//

#include "Movie.h"

int Movie::getKey() const {
    return _id;
}

int Movie::getYear() const {
    return _year;
}

const char *Movie::getTitle() const {
    return _title;
}
