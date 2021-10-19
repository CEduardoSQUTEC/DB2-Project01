//
// Created by ceduardosq on 10/14/21.
//

#ifndef FILEORGANIZATION_MOVIE_H
#define FILEORGANIZATION_MOVIE_H

struct Movie {
    int _id;
    int _year;
    char _title[250];

    int getKey() const;
    int getYear() const;
    const char *getTitle() const;
};


#endif //FILEORGANIZATION_MOVIE_H
