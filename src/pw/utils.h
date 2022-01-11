#include <spa/param/props.h>
#include <spa/param/param.h>
#include <spa/pod/vararg.h>
#include <spa/pod/builder.h>
#include <QtDebug>
#include <iostream>
#include <spa/utils/type-info.h>
#include <spa/utils/dict.h>

void print_dict(const struct spa_dict *dictionary)
{
   const struct spa_dict_item *item;
   spa_dict_for_each(item, dictionary) {
       std::cout << '\t' << item->key << ":" << item->value << std::endl;
   }
}

uint32_t spa_dict_get_u32(const spa_dict *props, const char* key)
{
    const char* str = spa_dict_lookup(props, key);
    if (str) {
        try { return std::stoul(str); }
        catch (const std::invalid_argument&) {}
        catch (const std::out_of_range&) {}
    }
    return -1;
}
