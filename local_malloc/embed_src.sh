# Quick and dirty tool to embed a file as C++ source code.
#
# @author John Kloosterman
# @date April 13, 2013
#
# Usage:
#  embed_src.sh <variable name> <file to embed>
#
# C++ source is outputted to standard output.
#

echo "#include <string>"
echo "std::string $2 = "
sed "s/\\\/\\\\\\\/g" $1 | sed 's/"/\\"/g' | sed -e 's/.*/\"&\\n\"/'
echo ";"
