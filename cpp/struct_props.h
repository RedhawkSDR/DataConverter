/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK Basic Components DataConverter.
 *
 * REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT
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
#include <CF/cf.h>
#include <ossie/PropertyMap.h>

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
    const redhawk::PropertyMap& props = redhawk::PropertyMap::cast(*temp);
    if (props.contains("scaleOutput::charPort")) {
        if (!(props["scaleOutput::charPort"] >>= s.charPort)) return false;
    }
    if (props.contains("scaleOutput::octetPort")) {
        if (!(props["scaleOutput::octetPort"] >>= s.octetPort)) return false;
    }
    if (props.contains("scaleOutput::shortPort")) {
        if (!(props["scaleOutput::shortPort"] >>= s.shortPort)) return false;
    }
    if (props.contains("scaleOutput::ushortPort")) {
        if (!(props["scaleOutput::ushortPort"] >>= s.ushortPort)) return false;
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const scaleOutput_struct& s) {
    redhawk::PropertyMap props;
 
    props["scaleOutput::charPort"] = s.charPort;
 
    props["scaleOutput::octetPort"] = s.octetPort;
 
    props["scaleOutput::shortPort"] = s.shortPort;
 
    props["scaleOutput::ushortPort"] = s.ushortPort;
    a <<= props;
}

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
}

inline bool operator!= (const scaleOutput_struct& s1, const scaleOutput_struct& s2) {
    return !(s1==s2);
}

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
    const redhawk::PropertyMap& props = redhawk::PropertyMap::cast(*temp);
    if (props.contains("normalize_floating_point::input")) {
        if (!(props["normalize_floating_point::input"] >>= s.input)) return false;
    }
    if (props.contains("normalize_floating_point::output")) {
        if (!(props["normalize_floating_point::output"] >>= s.output)) return false;
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const normalize_floating_point_struct& s) {
    redhawk::PropertyMap props;
 
    props["normalize_floating_point::input"] = s.input;
 
    props["normalize_floating_point::output"] = s.output;
    a <<= props;
}

inline bool operator== (const normalize_floating_point_struct& s1, const normalize_floating_point_struct& s2) {
    if (s1.input!=s2.input)
        return false;
    if (s1.output!=s2.output)
        return false;
    return true;
}

inline bool operator!= (const normalize_floating_point_struct& s1, const normalize_floating_point_struct& s2) {
    return !(s1==s2);
}

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
    const redhawk::PropertyMap& props = redhawk::PropertyMap::cast(*temp);
    if (props.contains("floatingPointRange::minimum")) {
        if (!(props["floatingPointRange::minimum"] >>= s.minimum)) return false;
    }
    if (props.contains("floatingPointRange::maximum")) {
        if (!(props["floatingPointRange::maximum"] >>= s.maximum)) return false;
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const floatingPointRange_struct& s) {
    redhawk::PropertyMap props;
 
    props["floatingPointRange::minimum"] = s.minimum;
 
    props["floatingPointRange::maximum"] = s.maximum;
    a <<= props;
}

inline bool operator== (const floatingPointRange_struct& s1, const floatingPointRange_struct& s2) {
    if (s1.minimum!=s2.minimum)
        return false;
    if (s1.maximum!=s2.maximum)
        return false;
    return true;
}

inline bool operator!= (const floatingPointRange_struct& s1, const floatingPointRange_struct& s2) {
    return !(s1==s2);
}

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
    const redhawk::PropertyMap& props = redhawk::PropertyMap::cast(*temp);
    if (props.contains("advancedSRI::update_col_rf_keyword")) {
        if (!(props["advancedSRI::update_col_rf_keyword"] >>= s.update_col_rf_keyword)) return false;
    }
    if (props.contains("advancedSRI::update_chan_rf_keyword")) {
        if (!(props["advancedSRI::update_chan_rf_keyword"] >>= s.update_chan_rf_keyword)) return false;
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const advancedSRI_struct& s) {
    redhawk::PropertyMap props;
 
    props["advancedSRI::update_col_rf_keyword"] = s.update_col_rf_keyword;
 
    props["advancedSRI::update_chan_rf_keyword"] = s.update_chan_rf_keyword;
    a <<= props;
}

inline bool operator== (const advancedSRI_struct& s1, const advancedSRI_struct& s2) {
    if (s1.update_col_rf_keyword!=s2.update_col_rf_keyword)
        return false;
    if (s1.update_chan_rf_keyword!=s2.update_chan_rf_keyword)
        return false;
    return true;
}

inline bool operator!= (const advancedSRI_struct& s1, const advancedSRI_struct& s2) {
    return !(s1==s2);
}

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
    const redhawk::PropertyMap& props = redhawk::PropertyMap::cast(*temp);
    if (props.contains("transformProperties::tune_fs_over_4")) {
        if (!(props["transformProperties::tune_fs_over_4"] >>= s.tune_fs_over_4)) return false;
    }
    if (props.contains("transformProperties::fftSize")) {
        if (!(props["transformProperties::fftSize"] >>= s.fftSize)) return false;
    }
    if (props.contains("transformProperties::overlap_percentage")) {
        if (!(props["transformProperties::overlap_percentage"] >>= s.overlap_percentage)) return false;
    }
    if (props.contains("transformProperties::lowCutoff")) {
        if (!(props["transformProperties::lowCutoff"] >>= s.lowCutoff)) return false;
    }
    if (props.contains("transformProperties::highCutoff")) {
        if (!(props["transformProperties::highCutoff"] >>= s.highCutoff)) return false;
    }
    if (props.contains("transformProperties::transitionWidth")) {
        if (!(props["transformProperties::transitionWidth"] >>= s.transitionWidth)) return false;
    }
    return true;
}

inline void operator<<= (CORBA::Any& a, const transformProperties_struct& s) {
    redhawk::PropertyMap props;
 
    props["transformProperties::tune_fs_over_4"] = s.tune_fs_over_4;
 
    props["transformProperties::fftSize"] = s.fftSize;
 
    props["transformProperties::overlap_percentage"] = s.overlap_percentage;
 
    props["transformProperties::lowCutoff"] = s.lowCutoff;
 
    props["transformProperties::highCutoff"] = s.highCutoff;
 
    props["transformProperties::transitionWidth"] = s.transitionWidth;
    a <<= props;
}

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
}

inline bool operator!= (const transformProperties_struct& s1, const transformProperties_struct& s2) {
    return !(s1==s2);
}

#endif // STRUCTPROPS_H
