TEMPLATE = subdirs

SUBDIRS += \
    navbar \
    navbardemo \
    designerplugin
    
example.depends = navbar
designerplugin.depends = navbar
