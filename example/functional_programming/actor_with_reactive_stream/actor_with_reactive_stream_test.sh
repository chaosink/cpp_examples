#!/bin/sh

# Invalid message which is empty.
echo '' \
	| nc -q1 127.0.0.1 42042

# Invalid message which starts with '#'.
echo '#hello' \
	| nc -q1 127.0.0.1 42042

echo 'hello' \
	| nc -q1 127.0.0.1 42042

echo '{}' \
	| nc -q1 127.0.0.1 42042

echo '{"FirstURL" : "http://www.iso.org/","Text" : "ISO"}' \
	| nc -q1 127.0.0.1 42042

echo '{"FirstURL" : "http://isocpp.org/","Text" : "ISO C++ -- Official site"}' \
	| nc -q1 127.0.0.1 42042
