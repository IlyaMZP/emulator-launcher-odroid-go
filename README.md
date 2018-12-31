# emulator-launcher-odroid-go
Open Source Emulator Launcher for Odroid Go to replace Springboard

# Build
You can use prebuilt "Go-Play.fw" binary or make it yourself using mkfw tool for odroid go
`mkfw "Go-Play (20180726)" tile.raw 0 16 1048576 springboard emulator-launcher-odroid-go.bin 0 17 1048576 nesemu nesemu.bin 0 18 1048576 gnuboy gnuboy.bin 0 19 2097152 smsplusgx smsplusgx.bin`

# Credits
Huge thanks to @jkent [odroid-go-launcher](https://github.com/jkent/odroid-go-launcher)
