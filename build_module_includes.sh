#!/bin/bash
echo "dsp module headers:"
for path in `find modules/ -type f -name "modules_*.h" | grep "\.h" | grep -v '#' | grep -v '~'`
do
    echo "#include \"$path\""
done

echo "dsp module osc handler headers:"
for path in `find modules/ -type f -name "osc_modules_*.h" | grep "\.h" | grep -v '#' | grep -v '~'`
do
    echo "#include \"$path\""
done
