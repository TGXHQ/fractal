#include <eosio/chain/token_snapshot.hpp>
#include <eosio/chain/exceptions.hpp>
#include <fc/scoped_exit.hpp>

namespace eosio { namespace chain {

uint64_t iostream_token_snapshot::end_marker = std::numeric_limits<uint64_t>::max();

token_snapshot_writer::token_snapshot_writer(std::ostream& snapshot)
:snapshot(snapshot)
,pos(snapshot.tellp())
{
}

void token_snapshot_writer::write_file_head()
{
   // write magic number
   auto totem = token_snapshot_head::magic_number;
   snapshot.write((char*)&totem, sizeof(totem));

   // write version
   auto version = token_snapshot_head::current_snapshot_version;
   snapshot.write((char*)&version, sizeof(version));
}

void token_snapshot_writer::finalize()
{
   // write a placeholder for the section size
   snapshot.write((char*)&end_marker, sizeof(end_marker));
}

token_snapshot_reader::token_snapshot_reader(std::istream& snapshot)
:snapshot(snapshot)
,pos(snapshot.tellg())
{
}

void token_snapshot_reader::exceptions()
{
    snapshot.exceptions(std::istream::failbit|std::istream::eofbit);
}

void token_snapshot_reader::seekg_valid_pos()
{
    std::streampos file_head_size = std::streamoff(sizeof(token_snapshot_head::magic_number)+sizeof(token_snapshot_head::current_snapshot_version));
    snapshot.seekg(file_head_size,std::ios::beg);
}

std::streampos token_snapshot_reader::get_file_size()
{
    snapshot.seekg(0,std::ios::end);
    std::streampos file_size = snapshot.tellg();
    snapshot.seekg(0,std::ios::beg);
    return file_size;
}

std::streampos token_snapshot_reader::get_valid_file_size()
{
    std::streampos file_size = get_file_size();

    std::streampos file_head_size = std::streamoff(sizeof(token_snapshot_head::magic_number)+sizeof(token_snapshot_head::current_snapshot_version));
    std::streampos end_maker_size = std::streamoff(sizeof(end_marker));
    snapshot.seekg(file_head_size,std::ios::beg);

    std::streampos valid_file_size = file_size - file_head_size - end_maker_size;
    snapshot.seekg(0,std::ios::beg);
    return valid_file_size;
}

std::streampos token_snapshot_reader::get_end_pos()
{
    std::streampos end_marker_size = std::streamoff(sizeof(end_marker));
    snapshot.seekg(-end_marker_size,std::ios::end);
    std::streampos pos = snapshot.tellg();
    snapshot.seekg(0,std::ios::beg);
    return pos;
}

void token_snapshot_reader::validate()
{
  snapshot.exceptions(std::istream::failbit|std::istream::eofbit);

  try {
    // validate totem
    auto expected_totem = token_snapshot_head::magic_number;
    decltype(expected_totem) actual_totem;
    snapshot.read((char*)&actual_totem, sizeof(actual_totem));
    EOS_ASSERT(actual_totem == expected_totem, token_snapshot_validate_exception,
             "Validate token snapshot exception");

    // validate version
    auto expected_version = token_snapshot_head::current_snapshot_version;
    decltype(expected_version) actual_version;
    snapshot.read((char*)&actual_version, sizeof(actual_version));
    EOS_ASSERT(actual_version == expected_version, token_snapshot_validate_exception,
                "Validate token snapshot exception");
    // validate end_marker
    std::streampos end_maker_size = std::streamoff(sizeof(iostream_token_snapshot::end_marker));
    std::streampos file_size = get_file_size();
    snapshot.seekg((file_size-end_maker_size),std::ios::beg);

    uint64_t end_marker = 0;
    snapshot.read((char*)&end_marker,sizeof(end_marker));
    if(end_marker != iostream_token_snapshot::end_marker){
      EOS_ASSERT(false, token_snapshot_validate_exception,
                "Validate token snapshot exception");
    }

  } catch( const std::exception& e ) {  
    EOS_ASSERT(false, token_snapshot_IO_exception,
                "Binary snapshot throw IO exception (${what})",("what",e.what()));
  }
}

}}