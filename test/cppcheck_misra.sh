
cppcheck --include=/usr/local/include/tcl.h --xml --suppressions-list=suppressions.txt\
        -DTCL_OK=0 -DTCL_ERROR=1 -DTCL_RETURN=2 -DTCL_BREAK=3 -DTCL_CONTINUE=4 -DPACKAGE_NAME=\"argparse\" \
        -DPACKAGE_VERSION=0.61 --check-level=exhaustive --inline-suppr --addon=misra --enable=style \
        ../generic --dump 2>misra_results.xml
python3 /usr/local/share/Cppcheck/addons/misra.py --rule-texts=/home/georgtree/docs/misra/MISRA-C-2025_Rules.txt \
        ../generic/*.dump > results.txt
