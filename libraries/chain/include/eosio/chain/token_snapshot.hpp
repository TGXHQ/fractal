#pragma once

#include <eosio/chain/exceptions.hpp>
#include <iostream>

namespace eosio { namespace chain {

class iostream_token_snapshot
{
    protected:
    struct token_snapshot_head{
        static const uint32_t magic_number = 0x30510550;
        static const uint32_t current_snapshot_version = 1;
    };
    static uint64_t end_marker;
};

class token_snapshot_writer:public iostream_token_snapshot
{
    public:
    explicit token_snapshot_writer(std::ostream& snapshot);
    void finalize();
    void write_file_head();
    auto& write(const char* d, size_t s)
    {
        return snapshot.write(d,s);
    }
    private:
    std::ostream&  snapshot;
    std::streampos pos;
};

class token_snapshot_reader:public iostream_token_snapshot
{
    public:
    explicit token_snapshot_reader(std::istream& snapshot);
    void validate();
    void exceptions();
    void seekg_valid_pos();
    std::streampos get_file_size();
    std::streampos get_valid_file_size();
    std::streampos get_end_pos();
    auto& read(char* d, size_t s)
    {
        return snapshot.read(d,s);
    }
    std::streampos get_pos()
    {
        return snapshot.tellg();
    }
   
    private:
    std::istream&  snapshot;
    std::streampos pos;
};

}}