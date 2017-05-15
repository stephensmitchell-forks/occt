# Definitions for a product: Helix

# List of toolkits 
proc Helix:toolkits { } {
    return [list TKHelix]
}

# List of non-toolkits (resource units, executables etc., with associated info)
proc Helix:ressources { } {
    return {}
}

# Product name 
proc Helix:name { } {
    return Helix
}

# And short alias
proc Helix:alias { } {
    return Helix
}

# Dependency on other products
proc Helix:depends { } {
    return {}
}

proc Helix:CompileWith { } {
}

proc Helix:LinksoWith { } {
}

proc Helix:Export { } {
    return [list source runtime wokadm api]
}
