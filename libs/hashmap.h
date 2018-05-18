#ifndef HASHMAP_H_DEF
#define HASHMAP_H_DEF

#include "types.h"
#include "hash.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


namespace Hashmap
{

  template <typename key_type, typename value_type, u32 n_slots=512, u32 initval=2341234675>
  struct Hashmap
  {
    struct HashSlot
    {
      key_type *key;
      u32 key_length;

      value_type value;

      HashSlot *next;

    } map[n_slots];

    struct
    {
      // Only used for collecting stats.  Not used for functionality.  Not guaranteed to be accurate.
      u32 collision_count;
      u32 n_items;
    } stats;

    // Constructor to zero members
    Hashmap()
    {
      memset(this->map, 0, n_slots*sizeof(HashSlot));
    }

  };


  template <typename key_type>
  b32
  keys_equal(key_type *key_a, u32 key_a_length, const key_type *key_b, u32 key_b_length)
  {
    b32 result = true;

    result &= (key_a_length == key_b_length);

    if (result)
    {
      for (u32 i = 0;
           i < key_a_length;
           ++i)
      {
        result &= (key_a[i] == key_b[i]);

        if (!result)
        {
          break;
        }
      }
    }

    return result;
  }


  // Returns the first-level hashslot refereed to by the key, __does not__ check for collisions!
  //
  template <typename key_type, typename value_type, u32 n_slots, u32 initval>
  typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot *
  get_slot(Hashmap<key_type, value_type, n_slots, initval>& hashmap, const key_type *key, u32 key_length = 1)
  {
    u32 hash = hashlittle((void *)key, sizeof(key_type) * key_length, initval);

    auto *slot = hashmap.map + (hash % n_slots);
    return slot;
  }


  // Follows the linked list in a HashSlot searching for the slot matching key.
  // If a matching slot is not found, returns NULL.
  //
  // previous_slot_result is filled in with a pointer to the slot pointing to the returned slot, if the returned slot is not the first level slot.
  //
  template <typename key_type, typename value_type, u32 n_slots, u32 initval>
  typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot *
  find_collided_value(typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot *slot, const key_type *key, u32 key_length = 1, typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot **previous_slot_result = NULL)
  {
    b32 matching_key = false;

    while (!matching_key &&
           slot != NULL)
    {
      matching_key = keys_equal(slot->key, slot->key_length, key, key_length);

      if (!matching_key)
      {
        if (previous_slot_result != NULL)
        {
          *previous_slot_result = slot;
        }

        slot = slot->next;
      }
    }

    return slot;
  }


  // Gets an existing hash slot, returns NULL if key does not exist.
  //
  template <typename key_type, typename value_type, u32 n_slots, u32 initval>
  value_type *
  get(Hashmap<key_type, value_type, n_slots, initval>& hashmap, const key_type *key, u32 key_length = 1)
  {
    value_type *result = 0;

    auto *slot = get_slot(hashmap, key, key_length);
    slot = find_collided_value<key_type, value_type, n_slots, initval>(slot, key, key_length);

    if (slot != NULL)
    {
      result = &slot->value;
    }

    return result;
  }


  // Returns a reference to the either newly-created slot, or existing slot.
  //
  template <typename key_type, typename value_type, u32 n_slots, u32 initval>
  value_type &
  set(Hashmap<key_type, value_type, n_slots, initval>& hashmap, const key_type *key, u32 key_length = 1)
  {
    value_type *result;

    b32 new_slot = false;

    typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot *slot = get_slot(hashmap, key, key_length);
    typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot *previous_slot = NULL;

    // If the first slot is not empty, resolve any collision
    if (slot->key == NULL)
    {
      new_slot = true;
    }
    else
    {
      slot = find_collided_value<key_type, value_type, n_slots, initval>(slot, key, key_length, &previous_slot);
    }

    // Create new slot.
    if (slot == NULL)
    {
      hashmap.stats.collision_count += 1;

      new_slot = true;

      slot = (typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot *)malloc(sizeof(typename Hashmap<key_type, value_type, n_slots, initval>::HashSlot));

      assert(slot != NULL);  // Malloc failed

      // Update the previous item in the collision linked list
      if (previous_slot != NULL)
      {
        previous_slot->next = slot;
      }
    }

    assert(slot != NULL);

    if (new_slot)
    {
      memset(slot, 0, sizeof(typename Hashmap<key_type, value_type>::HashSlot));

      slot->key = (key_type *)malloc(key_length * sizeof(key_type));
      memcpy(slot->key, key, key_length * sizeof(key_type));
      slot->key_length = key_length;

      hashmap.stats.n_items += 1;
    }

    result = &slot->value;

    return *result;
  }


  template <typename key_type, typename value_type, u32 n_slots, u32 initval>
  void
  print_stats(Hashmap<key_type, value_type, n_slots, initval>& hashmap)
  {
    printf("%d items cause %d collisions in Hashmap of size %d.\n", hashmap.stats.n_items, hashmap.stats.collision_count, n_slots);
    printf("Collision rate:      %.2f%%\n", 100 * r32(hashmap.stats.collision_count) / r32(hashmap.stats.n_items));
    printf("Hashmap saturation:  %.2f%%\n", 100 * r32(hashmap.stats.n_items) / r32(n_slots));
    printf("\n");
  }


  static void
  self_test()
  {
    printf("# Self Test:\n");
    const u32 map_size = 2u << 15u;
    const u32 n_items = 10000;

    Hashmap<u32, u32, map_size> map = {};
    for (u32 i = 0;
         i < n_items;
         ++i)
    {
      set(map, &i) = 10;
      assert(get(map, &i) != NULL);
    }

    print_stats(map);
  }
};


#define KEY(c_string) c_string, strlen(c_string)


#endif