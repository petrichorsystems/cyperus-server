#!/bin/bash
rm -f osc.c
rm -f osc.h
mkdir -p build

cp templates/osc.h.template/osc.h.template.prefix osc.h

for module_type in `ls modules/ | grep -v "\."`
do
    for module_subtype in `ls modules/$module_type/ | grep -v "\."`
    do
	for module in `ls modules/$module_type/$module_subtype/ | grep -v "\."`
	do
	    osc_params=`cat "modules/$module_type/$module_subtype/$module/osc.params"`
	    add_func=`cat "modules/$module_type/$module_subtype/$module/osc_modules_${module_type}_${module_subtype}_${module}.h" | grep add | cut -d' ' -f2 | cut -d'(' -f1`
	    edit_func=`cat "modules/$module_type/$module_subtype/$module/osc_modules_${module_type}_${module_subtype}_${module}.h" | grep edit | cut -d' ' -f2 | cut -d'(' -f1`
	    echo "#include \"modules/$module_type/$module_subtype/$module/osc_modules_${module_type}_${module_subtype}_${module}.h\"" >> osc.h
	done
    done
done

cat templates/osc.h.template/osc.h.template.suffix >> osc.h

cat templates/osc.c.template/osc.c.template.prefix >> osc.c
for module_type in `ls modules/ | grep -v "\."`
do
    for module_subtype in `ls modules/$module_type/ | grep -v "\."`
    do
	for module in `ls modules/$module_type/$module_subtype/ | grep -v "\."`
	do
	    osc_params=`cat "modules/$module_type/$module_subtype/$module/osc.params"`
	    add_func=`cat "modules/$module_type/$module_subtype/$module/osc_modules_${module_type}_${module_subtype}_${module}.h" | grep add | cut -d' ' -f2 | cut -d'(' -f1`
	    edit_func=`cat "modules/$module_type/$module_subtype/$module/osc_modules_${module_type}_${module_subtype}_${module}.h" | grep edit | cut -d' ' -f2 | cut -d'(' -f1`

	    osc_method="add"
	    osc_path="/cyperus/$osc_method/module/$module_type/$module_subtype/$module"
	    handler_prefix="\"$osc_path\", \"$osc_params\""	  	    
	    echo "  lo_server_thread_add_method(lo_thread, $handler_prefix, $add_func, NULL);" >> osc.c
	    
	    osc_method="edit"
	    osc_path="/cyperus/$osc_method/module/$module_type/$module_subtype/$module"
	    handler_prefix="\"$osc_path\", \"$osc_params\""	  	    	    
	    echo "  lo_server_thread_add_method(lo_thread, $handler_prefix, $edit_func, NULL);" >> osc.c
	done
    done
done
cat templates/osc.c.template/osc.c.template.suffix >> osc.c

