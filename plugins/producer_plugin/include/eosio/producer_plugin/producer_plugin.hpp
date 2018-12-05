/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#pragma once

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_client_plugin/http_client_plugin.hpp>

#include <appbase/application.hpp>

namespace eosio {

using boost::signals2::signal;

class producer_plugin : public appbase::plugin<producer_plugin> {
public:
   APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_client_plugin))

   struct runtime_options {
      fc::optional<int32_t> max_transaction_time;
      fc::optional<int32_t> max_irreversible_block_age;
      fc::optional<int32_t> produce_time_offset_us;
      fc::optional<int32_t> last_block_time_offset_us;
      fc::optional<int32_t> subjective_cpu_leeway_us;
      fc::optional<double>  incoming_defer_ratio;
   };

   struct whitelist_blacklist {
      fc::optional< flat_set<account_name> > actor_whitelist;
      fc::optional< flat_set<account_name> > actor_blacklist;
      fc::optional< flat_set<account_name> > contract_whitelist;
      fc::optional< flat_set<account_name> > contract_blacklist;
      fc::optional< flat_set< std::pair<account_name, action_name> > > action_blacklist;
      fc::optional< flat_set<public_key_type> > key_blacklist;
   };

   struct greylist_params {
      std::vector<account_name> accounts;
   };

   struct integrity_hash_information {
      chain::block_id_type head_block_id;
      chain::digest_type   integrity_hash;
   };

   struct snapshot_information {
      chain::block_id_type head_block_id;
      std::string          snapshot_name;
   };

   struct token_snapshot_params {
      account_name code; //contranct account
      fc::optional<string> symbol;
   };

   struct token_snapshot_results{
       uint32_t head_block_num;
       std::string snapshot_path;
   };

   struct get_token_snapshot_params{
       account_name code;
       fc::optional<string> symbol;
       fc::optional<uint32_t> block_num;
   };

   struct get_token_snapshot_results{
       std::string account;
       std::string amount;
       std::string symbol;
   };

   producer_plugin();
   virtual ~producer_plugin();

   virtual void set_program_options(
      boost::program_options::options_description &command_line_options,
      boost::program_options::options_description &config_file_options
      ) override;

   bool                   is_producer_key(const chain::public_key_type& key) const;
   chain::signature_type  sign_compact(const chain::public_key_type& key, const fc::sha256& digest) const;

   virtual void plugin_initialize(const boost::program_options::variables_map& options);
   virtual void plugin_startup();
   virtual void plugin_shutdown();

   void pause();
   void resume();
   bool paused() const;
   void update_runtime_options(const runtime_options& options);
   runtime_options get_runtime_options() const;

   void add_greylist_accounts(const greylist_params& params);
   void remove_greylist_accounts(const greylist_params& params);
   greylist_params get_greylist() const;

   whitelist_blacklist get_whitelist_blacklist() const;
   void set_whitelist_blacklist(const whitelist_blacklist& params);

   integrity_hash_information get_integrity_hash() const;
   snapshot_information create_snapshot() const;

   token_snapshot_results create_token_snapshot(const token_snapshot_params& params) const;
   vector<get_token_snapshot_results> get_token_snapshot_info(const get_token_snapshot_params& params) const;

   template< typename T>
   void get_token_snapshot(std::shared_ptr<T>& reader ,vector<get_token_snapshot_results>& results) const;

   signal<void(const chain::producer_confirmation&)> confirmed_block;
private:
   std::shared_ptr<class producer_plugin_impl> my;
};

} //eosio

FC_REFLECT(eosio::producer_plugin::runtime_options, (max_transaction_time)(max_irreversible_block_age)(produce_time_offset_us)(last_block_time_offset_us)(subjective_cpu_leeway_us)(incoming_defer_ratio));
FC_REFLECT(eosio::producer_plugin::greylist_params, (accounts));
FC_REFLECT(eosio::producer_plugin::whitelist_blacklist, (actor_whitelist)(actor_blacklist)(contract_whitelist)(contract_blacklist)(action_blacklist)(key_blacklist) )
FC_REFLECT(eosio::producer_plugin::integrity_hash_information, (head_block_id)(integrity_hash))
FC_REFLECT(eosio::producer_plugin::snapshot_information, (head_block_id)(snapshot_name))
FC_REFLECT(eosio::producer_plugin::token_snapshot_params, (code)(symbol))
FC_REFLECT(eosio::producer_plugin::token_snapshot_results, (head_block_num)(snapshot_path))
FC_REFLECT(eosio::producer_plugin::get_token_snapshot_params, (code)(symbol)(block_num))
FC_REFLECT(eosio::producer_plugin::get_token_snapshot_results, (account)(amount)(symbol))
