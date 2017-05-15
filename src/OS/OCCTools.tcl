# Definitions for a product: Unfolding

# List of toolkits 
proc Unfolding:toolkits { } {
    return [list TKOCCTools TKOCCToolsTest]
}

# List of non-toolkits (resource units, executables etc., with associated info)
proc Unfolding:ressources { } {
    return {}
}

# Product name 
proc Unfolding:name { } {
    return Unfolding
}

# And short alias
proc Unfolding:alias { } {
    return Unfolding
}

# Dependency on other products
proc Unfolding:depends { } {
    return {}
}

proc Unfolding:CompileWith { } {
}

proc Unfolding:LinksoWith { } {
}

proc Unfolding:Export { } {
    return [list source runtime wokadm api]
}
