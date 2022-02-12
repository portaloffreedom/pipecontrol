/* 
 * This file is part of the pipecontrol project.
 * Copyright (c) 2022 Matteo De Carlo.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "utils.h"
#include <QtDebug>
#include <iostream>

void print_dict(const struct spa_dict *dictionary)
{
    const struct spa_dict_item *item;
    spa_dict_for_each(item, dictionary) {
        std::cout << '\t' << item->key << ":" << item->value << std::endl;
    }
}

uint32_t spa_dict_get_u32(const spa_dict *props, const char *key)
{
    const char* str = spa_dict_lookup(props, key);
    if (str) {
        try { return std::stoul(str); }
        catch (const std::invalid_argument&) {}
        catch (const std::out_of_range&) {}
    }
    return -1;
}

std::ostream& operator<< (std::ostream& out, const spa_pod& _pod)
{
    const spa_pod *pod = &_pod;
    switch (pod->type) {
        case SPA_TYPE_None:
            out << "[None]";
            break;
        case SPA_TYPE_Bool:
            out << SPA_POD_VALUE(spa_pod_bool, pod);
            break;
        case SPA_TYPE_Id:
            out << SPA_POD_VALUE(spa_pod_id, pod);
            break;
        case SPA_TYPE_Int:
            out << SPA_POD_VALUE(spa_pod_int, pod);
            break;
        case SPA_TYPE_Long:
            out << SPA_POD_VALUE(spa_pod_long, pod);
            break;
        case SPA_TYPE_Float:
            out << SPA_POD_VALUE(spa_pod_float, pod);
            break;
        case SPA_TYPE_Double:
            out << SPA_POD_VALUE(spa_pod_double, pod);
            break;
        case SPA_TYPE_String:
            out << "[string]";//SPA_POD_VALUE(spa_pod_string, pod);
            break;
        case SPA_TYPE_Bytes:
            out << "[bytes]";//SPA_POD_VALUE(spa_type_, pod);
            break;
        case SPA_TYPE_Rectangle:
            out << "[rectangle]";//SPA_POD_VALUE(spa_type_rec, pod);
            break;
        case SPA_TYPE_Fraction:
            out << "[fraction]";//SPA_POD_VALUE(spa_type_fraction, pod);
            break;
        case SPA_TYPE_Bitmap:
            out << "[bitmap]";//SPA_POD_VALUE(spa_type_bitmap, pod);
            break;
        case SPA_TYPE_Array:
            out << "[array]";//SPA_POD_VALUE(spa_type_array, pod);
            break;
        case SPA_TYPE_Struct:
            out << "[struct]";//SPA_POD_VALUE(SPA_TYPE_, pod);
            break;
        case SPA_TYPE_Object:
            out << "[object]";//SPA_POD_VALUE(SPA_TYPE_, pod);
            break;
        case SPA_TYPE_Sequence:
            out << "[sequence]";//SPA_POD_VALUE(SPA_TYPE_, pod);
            break;
        case SPA_TYPE_Pointer:
            out << "[pointer]";//SPA_POD_VALUE(SPA_TYPE_, pod);
            break;
        case SPA_TYPE_Fd:
            out << "[fd]";//SPA_POD_VALUE(SPA_TYPE_, pod);
            break;
        case SPA_TYPE_Choice:
            out << "[choice]";//SPA_POD_VALUE(SPA_TYPE_, pod);
            break;
        case SPA_TYPE_Pod:
            out << "[pod]";//SPA_POD_VALUE(SPA_TYPE_, pod);
            break;
        default:
            out << "{unknown}";
            break;
    }
    return out;
}
