#!/bin/sh

if [ x$1 = "x" ] ; then
      echo "$0 Usage:"
      echo "	--prefix=[Installation dir]			Sets the installation diretory to [Installation dir]"
      echo "	--with-libnetmeter=[libnetmeter location]	Sets the installation dir of libnetmeter"
      echo "	--with-ftgl=[ftgl location]			Sets the ftgl directory"
fi

OPTIONS_FILE=CMakeOptions.txt

if [ -e $OPTIONS_FILE ] ; then
  echo "Removing previous configuration"
  rm $OPTIONS_FILE
fi

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
      echo "set (CMAKE_INSTALL_PREFIX \"$my_value\")" >> $OPTIONS_FILE
      ;;
    --with-libnetmeter)
      echo "set (LIBNETMETER_BASE \"$my_value\")" >> $OPTIONS_FILE
      ;;
    --with-ftgl)
      echo "set (FTGL_INCLUDE_DIR \"$my_value\"/include)" >> $OPTIONS_FILE
      echo "set (FTGL_LIBRARIES \"$my_value\"/lib)" >> $OPTIONS_FILE
      ;;
    --help)
      echo "$0 Usage:"
      echo "	--prefix=[Installation dir]	Sets the installation diretory to [Installation dir]"
      echo "	--with-libnetmeter=[libnetmeter location]	Sets the installation dir of libnetmeter"
      ;;
    * )
      echo "Configuring with default options:"
      echo "Install directory set to /usr/local/netmetergui"
      echo "set (CMAKE_INSTALL_PREFIX \"/usr/local/netmetergui\")" >> $OPTIONS_FILE
      ;;
  esac
done

