mkdir -p build

echo "Compiling client..."
gcc src/client.c src/mjep.c src/config.h src/colors.h -o build/client

echo "Done."

echo "Compiling Server..."
gcc src/server.c src/mjep.c src/config.h src/colors.h -o build/server

echo "Done."

echo "Ready to run the application"
