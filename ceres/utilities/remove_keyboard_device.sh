set -e

GADGET_PATH="/sys/kernel/config/usb_gadget/g1"

if [ ! -d "$GADGET_PATH" ]; then
    echo "Gadget g1 not found"
    exit 1
fi

if [ ! -d "$GADGET_PATH/functions/hid.usb0" ]; then
    echo "HID keyboard function not found in g1"
    exit 0
fi

cd "$GADGET_PATH"

# Unbind from UDC
if [ -f UDC ]; then
    UDC_DEVICE=$(cat UDC)
    if [ -n "$UDC_DEVICE" ]; then
        echo "Unbinding from UDC: $UDC_DEVICE"
        echo "" > UDC
    fi
fi

# Remove symlink
if [ -L configs/c.1/hid.usb0 ]; then
    echo "Removing HID function link..."
    rm configs/c.1/hid.usb0
fi

# Remove HID function
if [ -d functions/hid.usb0 ]; then
    echo "Removing HID function..."
    rmdir functions/hid.usb0
fi

# Re-bind to UDC
if [ -n "$UDC_DEVICE" ]; then
    echo "Re-binding to UDC: $UDC_DEVICE"
    echo "$UDC_DEVICE" > UDC
fi

echo "HID keyboard function removed from g1"
