/**
 *
 * user_list.cpp
 *
 * A user in p2d
 *
 * Author: Sunwoo Na
 *
 */

#include <sstream>

#include <cryptlib.h>
#include <hex.h>
#include <sha.h>

#include "../include/user.h"

using CryptoPP::SHA256;
using namespace std;

namespace p2d {
///////// PASSWORD //////////
password::password(std::string_view pw)
    : hash { create_hash(pw) } { }

bool password::operator==(std::string_view other) const {
    return hash == password(other).hash;
}

std::string password::create_hash(std::string_view pw) {
    SHA256 hash;
    CryptoPP::byte digest[SHA256::DIGESTSIZE];

    hash.CalculateDigest(digest, (CryptoPP::byte *)pw.data(), pw.size());

    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach(new CryptoPP::StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return output;
}

istream &operator>>(istream &is, password &pw) {
    string input;
    is >> input;
    pw = password(input);
    return is;
}

//////////// USER ////////////
user::user(string_view name, string_view email, string_view id, password &&pw)
    : name { name }
    , email { email }
    , id { id }
    , pw { move(pw) } { }

user::user(const user &other) // drops password
    : name { other.name }
    , email { other.email }
    , id { other.id } { }

std::string user::get_name() const {
    return name;
}

std::string user::get_email() const {
    return email;
}

std::string user::get_id() const {
    return id;
}

user &user::operator=(const user &other) {
    name = other.name;
    email = other.email;
    id = other.id;
    // pw = other.pw; // password should not be copied
    return *this;
}

bool user::operator==(const user &other) const {
    return id == other.id;
}

strong_ordering user::operator<=>(const user &other) const {
    return id <=> other.id;
}
}