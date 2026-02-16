set -e

GADGET_PATH="/sys/kernel/config/usb_gadget/g1"

if [ ! -d "$GADGET_PATH" ]; then
  echo "Error: Gadget g1 not found"
  exit 1
fi

cd "$GADGET_PATH"

# Check if HID function already exists
if [ -d "functions/hid.usb0" ]; then
  echo "HID keyboard function already exists"
  # Check if it's linked
  if [ -L "configs/c.1/hid.usb0" ]; then
    echo "HID keyboard already configured"
    exit 0
  fi
else
  # Unbind from UDC temporarily
  UDC_DEVICE=$(cat UDC)
  echo "Unbinding from UDC: $UDC_DEVICE"
  echo "" >UDC

  # Create HID function
  echo "Creating HID keyboard function..."
  mkdir -p functions/hid.usb0
  echo 1 >functions/hid.usb0/protocol # Keyboard protocol
  echo 1 >functions/hid.usb0/subclass # Boot interface subclass
  echo 8 >functions/hid.usb0/report_length

  # Write HID Report Descriptor (standard keyboard)
  echo -ne \\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0 >functions/hid.usb0/report_desc

  # Link function to configuration
  echo "Linking HID function to configuration..."
  ln -s functions/hid.usb0 configs/c.1/

  # Re-bind to UDC
  echo "Re-binding to UDC: $UDC_DEVICE"
  echo "$UDC_DEVICE" >UDC
fi

echo "USB Keyboard function added successfully"
echo "Device available at /dev/hidg0"
