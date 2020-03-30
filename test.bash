#!/bin/bash

#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./tanacc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi

  rm tmp.s tmp
}

try 0 0
try 42 42
try 21 "5+20-4"
try 41 " 12 + 34 - 5 "
try 47 "5+6*7"
try 15 "5*( 9 - 6 )"
try 4  "(3+5) / 2"
try 6  "-3 * (-8 / +4)"

echo OK
