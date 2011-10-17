#!/bin/sh

if [ x$1 = "x" ] ; then
      echo "$0 Usage:"
      echo "	--prefix=[Installation dir]	Sets the installation diretory to [Installation dir]"
fi

OPTIONS_FILE=CMakeOptions.txt

for arg in "$@"
do
  my_argument=`echo $arg | cut -f 1 -d=`
  my_value=`echo $arg | cut -f 2 -d=`

  case "$my_argument" in
    --prefix)
      check_home=`echo $my_value | grep -e '~'`
      if [ "x$check_home" != "x" ] ; then
         my_value=${HOME}`echo $my_value | cut -d~ -f 2`
      fi
      echo "Setting installation directory to $my_value"
      echo "set (CMAKE_INSTALL_PREFIX \"$my_value\")" > $OPTIONS_FILE
      ;;
    --help)
      echo "$0 Usage:"
      echo "	--prefix=[Installation dir]	Sets the installation diretory to [Installation dir]"
      ;;
    * )
      echo "Configuring with default options:"
      echo "Install directory set to /usr/local/netmeter"
      echo "set (CMAKE_INSTALL_PREFIX \"/usr/local/netmeter\")" > $OPTIONS_FILE
      ;;
  esac
done
