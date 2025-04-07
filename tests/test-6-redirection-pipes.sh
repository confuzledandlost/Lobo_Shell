TEST=$(basename "${0%.sh}")
OUTPUT=$(realpath $(dirname $0))/output
OUT=$OUTPUT/$TEST.out
EOUT=$OUTPUT/$TEST.eout
LOG=$OUTPUT/$TEST.log
LOBO="./lobo_shell.x"
BASH=`which bash`

test "tests" != "$(basename $(dirname $(realpath $0)))" && { echo "FAIL: $0 not in 'tests'"; exit 1; }
! test -x $LOBO && { echo "FAIL: $LOBO must exist"; exit 2; }
mkdir -p $OUTPUT
rm -f $OUT $EOUT

#
# Test Case
#
cat >$LOG << 'EOF'
Commands:
---------
cat < /usr/share/dict/words | head -10 > tests/output/tmp
---------
EOF

#
# Test with LOBO
#
$LOBO >$OUT 2>&1 << 'EOF'
cat < /usr/share/dict/words | head -10 > tests/output/tmp
EOF

#
# Test with BASH
#
$BASH >$EOUT 2>&1 << 'EOF'
cat < /usr/share/dict/words | head -10 > tests/output/etmp
EOF

#
# Analyze results
#


diff $EOUT $OUT >> $LOG
echo "---------" >> $LOG

diff $EOUT $OUT >/dev/null || { echo "FAIL $TEST"; exit 1; }

diff tests/output/etmp tests/output/tmp >>$LOG 2>&1 || { echo "FAIL $TEST"; exit 2; }
echo "PASS $TEST"

rm -f tests/output/tmp tests/output/etmp
