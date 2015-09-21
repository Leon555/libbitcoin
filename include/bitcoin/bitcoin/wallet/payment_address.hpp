/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_WALLET_PAYMENT_ADDRESS_HPP
#define LIBBITCOIN_WALLET_PAYMENT_ADDRESS_HPP

#include <algorithm>
#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace wallet {
    
static BC_CONSTEXPR size_t payment_size = 1u + short_hash_size + checksum_size;
typedef byte_array<payment_size> payment;

/**
 * A class for working with Bitcoin addresses.
 * Version defaults apply to Bitcoin mainnet ONLY.
 */
class BC_API payment_address
{
public:
    static BC_CONSTEXPR uint8_t mainnet = 0x00;
    static BC_CONSTEXPR uint8_t mainnet_p2sh = 0x05;

    payment_address();
    payment_address(const payment& bytes);
    payment_address(const std::string& encoded);
    payment_address(const payment_address& other);
    payment_address(const short_hash& hash, uint8_t version=mainnet);
    payment_address(const chain::script& script, uint8_t version=mainnet_p2sh);
    payment_address(const ec_public& point, uint8_t version=mainnet,
        bool compressed=true);
    payment_address(const ec_secret& secret, uint8_t version=mainnet,
        bool compressed=true);

    /// Test for validity.
    operator const bool() const;

    /// Get the decoded representation (including version and checksum).
    operator const payment() const;

    /// Get the base58 encoded representation.
    std::string encoded() const;

    /// Get the version.
    uint8_t version() const;

    /// Get the ripemd hash.
    const short_hash& hash() const;

private:
    static payment_address from_payment(const payment& decoded);
    static payment_address from_string(const std::string& encoded);
    static payment_address from_public(const ec_public& point,
        uint8_t version, bool compressed);
    static payment_address from_secret(const ec_secret& secret,
        uint8_t version, bool compressed);

    bool valid_;
    uint8_t version_;
    short_hash hash_;
};


/**
 * Override the equality operator to compare to addresses.
 * Compares only the hash value.
 */
BC_API bool operator==(const payment_address& left,
    const payment_address& right);

/**
 * Extract a payment address from an input or output script.
 * The address will be invalid if and only if the script type is not
 * supported or the script is itself invalid.
 */
BC_API payment_address extract_address(const chain::script& script);

} // namspace wallet
} // namspace libbitcoin

// Allow payment_address to be in indexed in std::*map classes.
namespace std
{
    template <>
    struct BC_API hash<bc::wallet::payment_address>
    {
        size_t operator()(const bc::wallet::payment_address& address) const
        {
            // Create a string of the form [address-version|address-hash].
            std::string buffer;
            buffer.resize(bc::short_hash_size + 1);
            buffer[0] = address.version();
            const auto& hash = address.hash();
            std::copy(hash.begin(), hash.end(), buffer.begin() + 1);

            std::hash<std::string> functor;
            return functor(buffer);
        }
    };

} // namspace std

#endif
