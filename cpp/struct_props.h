/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of FilterDecimate.
 *
 * FilterDecimate is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * FilterDecimate is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#ifndef STRUCTPROPS_H
#define STRUCTPROPS_H

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

*******************************************************************************************/

#include <ossie/CorbaUtils.h>

struct scaleOutput_struct {
    scaleOutput_struct ()
    {
        charPort = true;
        octetPort = true;
        shortPort = true;
        ushortPort = true;
    };

    static std::string getId() {
        return std::string("scaleOutput");
    };

    bool charPort;
    bool octetPort;
    bool shortPort;
    bool ushortPort;
};

inline bool operator>>= (const CORBA::Any& a, scaleOutput_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("scaleOutput::charPort", props[idx].id)) {
            if (!(props[idx].value >>= s.charPort)) return false;
        }
        else if (!strcmp("scaleOutput::octetPort", props[idx].id)) {
            if (!(props[idx].value >>= s.octetPort)) return false;
        }
        else if (!strcmp("scaleOutput::shortPort", props[idx].id)) {
            if (!(props[idx].value >>= s.shortPort)) return false;
        }
        else if (!strcmp("scaleOutput::ushortPort", props[idx].id)) {
            if (!(props[idx].value >>= s.ushortPort)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const scaleOutput_struct& s) {
    CF::Properties props;
    props.length(4);
    props[0].id = CORBA::string_dup("scaleOutput::charPort");
    props[0].value <<= s.charPort;
    props[1].id = CORBA::string_dup("scaleOutput::octetPort");
    props[1].value <<= s.octetPort;
    props[2].id = CORBA::string_dup("scaleOutput::shortPort");
    props[2].value <<= s.shortPort;
    props[3].id = CORBA::string_dup("scaleOutput::ushortPort");
    props[3].value <<= s.ushortPort;
    a <<= props;
};

inline bool operator== (const scaleOutput_struct& s1, const scaleOutput_struct& s2) {
    if (s1.charPort!=s2.charPort)
        return false;
    if (s1.octetPort!=s2.octetPort)
        return false;
    if (s1.shortPort!=s2.shortPort)
        return false;
    if (s1.ushortPort!=s2.ushortPort)
        return false;
    return true;
};

inline bool operator!= (const scaleOutput_struct& s1, const scaleOutput_struct& s2) {
    return !(s1==s2);
};

struct normalize_floating_point_struct {
    normalize_floating_point_struct ()
    {
        input = true;
        output = true;
    };

    static std::string getId() {
        return std::string("normalize_floating_point");
    };

    bool input;
    bool output;
};

inline bool operator>>= (const CORBA::Any& a, normalize_floating_point_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("normalize_floating_point::input", props[idx].id)) {
            if (!(props[idx].value >>= s.input)) return false;
        }
        else if (!strcmp("normalize_floating_point::output", props[idx].id)) {
            if (!(props[idx].value >>= s.output)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const normalize_floating_point_struct& s) {
    CF::Properties props;
    props.length(2);
    props[0].id = CORBA::string_dup("normalize_floating_point::input");
    props[0].value <<= s.input;
    props[1].id = CORBA::string_dup("normalize_floating_point::output");
    props[1].value <<= s.output;
    a <<= props;
};

inline bool operator== (const normalize_floating_point_struct& s1, const normalize_floating_point_struct& s2) {
    if (s1.input!=s2.input)
        return false;
    if (s1.output!=s2.output)
        return false;
    return true;
};

inline bool operator!= (const normalize_floating_point_struct& s1, const normalize_floating_point_struct& s2) {
    return !(s1==s2);
};

struct floatingPointRange_struct {
    floatingPointRange_struct ()
    {
        minimum = 0.0;
        maximum = 3.402823466e38;
    };

    static std::string getId() {
        return std::string("floatingPointRange");
    };

    double minimum;
    double maximum;
};

inline bool operator>>= (const CORBA::Any& a, floatingPointRange_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("floatingPointRange::minimum", props[idx].id)) {
            if (!(props[idx].value >>= s.minimum)) return false;
        }
        else if (!strcmp("floatingPointRange::maximum", props[idx].id)) {
            if (!(props[idx].value >>= s.maximum)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const floatingPointRange_struct& s) {
    CF::Properties props;
    props.length(2);
    props[0].id = CORBA::string_dup("floatingPointRange::minimum");
    props[0].value <<= s.minimum;
    props[1].id = CORBA::string_dup("floatingPointRange::maximum");
    props[1].value <<= s.maximum;
    a <<= props;
};

inline bool operator== (const floatingPointRange_struct& s1, const floatingPointRange_struct& s2) {
    if (s1.minimum!=s2.minimum)
        return false;
    if (s1.maximum!=s2.maximum)
        return false;
    return true;
};

inline bool operator!= (const floatingPointRange_struct& s1, const floatingPointRange_struct& s2) {
    return !(s1==s2);
};

struct doublePointRange_struct {
    doublePointRange_struct ()
    {
    };

    static std::string getId() {
        return std::string("doublePointRange");
    };

    double minimum;
    double maximum;
};

inline bool operator>>= (const CORBA::Any& a, doublePointRange_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("doublePointRange::minimum", props[idx].id)) {
            if (!(props[idx].value >>= s.minimum)) return false;
        }
        else if (!strcmp("doublePointRange::maximum", props[idx].id)) {
            if (!(props[idx].value >>= s.maximum)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const doublePointRange_struct& s) {
    CF::Properties props;
    props.length(2);
    props[0].id = CORBA::string_dup("doublePointRange::minimum");
    props[0].value <<= s.minimum;
    props[1].id = CORBA::string_dup("doublePointRange::maximum");
    props[1].value <<= s.maximum;
    a <<= props;
};

inline bool operator== (const doublePointRange_struct& s1, const doublePointRange_struct& s2) {
    if (s1.minimum!=s2.minimum)
        return false;
    if (s1.maximum!=s2.maximum)
        return false;
    return true;
};

inline bool operator!= (const doublePointRange_struct& s1, const doublePointRange_struct& s2) {
    return !(s1==s2);
};

struct advancedSRI_struct {
    advancedSRI_struct ()
    {
        update_col_rf_keyword = true;
        update_chan_rf_keyword = true;
    };

    static std::string getId() {
        return std::string("advancedSRI");
    };

    bool update_col_rf_keyword;
    bool update_chan_rf_keyword;
};

inline bool operator>>= (const CORBA::Any& a, advancedSRI_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("advancedSRI::update_col_rf_keyword", props[idx].id)) {
            if (!(props[idx].value >>= s.update_col_rf_keyword)) return false;
        }
        else if (!strcmp("advancedSRI::update_chan_rf_keyword", props[idx].id)) {
            if (!(props[idx].value >>= s.update_chan_rf_keyword)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const advancedSRI_struct& s) {
    CF::Properties props;
    props.length(2);
    props[0].id = CORBA::string_dup("advancedSRI::update_col_rf_keyword");
    props[0].value <<= s.update_col_rf_keyword;
    props[1].id = CORBA::string_dup("advancedSRI::update_chan_rf_keyword");
    props[1].value <<= s.update_chan_rf_keyword;
    a <<= props;
};

inline bool operator== (const advancedSRI_struct& s1, const advancedSRI_struct& s2) {
    if (s1.update_col_rf_keyword!=s2.update_col_rf_keyword)
        return false;
    if (s1.update_chan_rf_keyword!=s2.update_chan_rf_keyword)
        return false;
    return true;
};

inline bool operator!= (const advancedSRI_struct& s1, const advancedSRI_struct& s2) {
    return !(s1==s2);
};

struct transformProperties_struct {
    transformProperties_struct ()
    {
        tune_fs_over_4 = false;
        fftSize = 2048;
        overlap_percentage = 25;
        lowCutoff = .001;
        highCutoff = .499;
        transitionWidth = .001;
    };

    static std::string getId() {
        return std::string("transformProperties");
    };

    bool tune_fs_over_4;
    CORBA::Long fftSize;
    double overlap_percentage;
    double lowCutoff;
    double highCutoff;
    double transitionWidth;
};

inline bool operator>>= (const CORBA::Any& a, transformProperties_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("transformProperties::tune_fs_over_4", props[idx].id)) {
            if (!(props[idx].value >>= s.tune_fs_over_4)) return false;
        }
        else if (!strcmp("transformProperties::fftSize", props[idx].id)) {
            if (!(props[idx].value >>= s.fftSize)) return false;
        }
        else if (!strcmp("transformProperties::overlap_percentage", props[idx].id)) {
            if (!(props[idx].value >>= s.overlap_percentage)) return false;
        }
        else if (!strcmp("transformProperties::lowCutoff", props[idx].id)) {
            if (!(props[idx].value >>= s.lowCutoff)) return false;
        }
        else if (!strcmp("transformProperties::highCutoff", props[idx].id)) {
            if (!(props[idx].value >>= s.highCutoff)) return false;
        }
        else if (!strcmp("transformProperties::transitionWidth", props[idx].id)) {
            if (!(props[idx].value >>= s.transitionWidth)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const transformProperties_struct& s) {
    CF::Properties props;
    props.length(6);
    props[0].id = CORBA::string_dup("transformProperties::tune_fs_over_4");
    props[0].value <<= s.tune_fs_over_4;
    props[1].id = CORBA::string_dup("transformProperties::fftSize");
    props[1].value <<= s.fftSize;
    props[2].id = CORBA::string_dup("transformProperties::overlap_percentage");
    props[2].value <<= s.overlap_percentage;
    props[3].id = CORBA::string_dup("transformProperties::lowCutoff");
    props[3].value <<= s.lowCutoff;
    props[4].id = CORBA::string_dup("transformProperties::highCutoff");
    props[4].value <<= s.highCutoff;
    props[5].id = CORBA::string_dup("transformProperties::transitionWidth");
    props[5].value <<= s.transitionWidth;
    a <<= props;
};

inline bool operator== (const transformProperties_struct& s1, const transformProperties_struct& s2) {
    if (s1.tune_fs_over_4!=s2.tune_fs_over_4)
        return false;
    if (s1.fftSize!=s2.fftSize)
        return false;
    if (s1.overlap_percentage!=s2.overlap_percentage)
        return false;
    if (s1.lowCutoff!=s2.lowCutoff)
        return false;
    if (s1.highCutoff!=s2.highCutoff)
        return false;
    if (s1.transitionWidth!=s2.transitionWidth)
        return false;
    return true;
};

inline bool operator!= (const transformProperties_struct& s1, const transformProperties_struct& s2) {
    return !(s1==s2);
};

#endif // STRUCTPROPS_H
