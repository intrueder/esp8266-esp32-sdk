#ifndef _COLORCONTROLLER_H_
#define _COLORCONTROLLER_H_

/**
 * @brief ColorController
 * @ingroup Controller
 **/
template <typename T>
class ColorController {
  public:

    /**
     * @brief Callback definition for onColor function
     * 
     * Gets called when device receive a `setColor` request \n
     * @param[in]   deviceId    String which contains the ID of device
     * @param[in]   r           Byte value for red
     * @param[in]   g           Byte value for green
     * @param[in]   b           Byte value for blue
     * @param[out]  r           Byte value for red
     * @param[out]  g           Byte value for green
     * @param[out]  b           Byte value for blue
     * @return      the success of the request
     * @retval      true        request handled properly
     * @retval      false       request was not handled properly because of some error
     * @section ColorCallback Example-Code
     * @snippet callbacks.cpp onColor
     **/
    using ColorCallback = std::function<bool(const String &, byte &, byte &, byte &)>;

    void onColor(ColorCallback cb);
    bool sendColorEvent(byte r, byte g, byte b, String cause = "PHYSICAL_INTERACTION");

  protected:
    bool handleColorController(const String &action, JsonObject &request_value, JsonObject &response_value);

  private:
    ColorCallback colorCallback;
};


/**
 * @brief Set callback function for `setColor` request
 * 
 * @param cb Function pointer to a `ColorCallback` function
 * @return void
 * @see ColorCallback
 **/
template <typename T>
void ColorController<T>::onColor(ColorCallback cb) {
  colorCallback = cb;
}

/**
 * @brief Send `setColor` event to SinricPro Server indicating actual color
 * 
 * @param r       Byte value for red
 * @param g       Byte value for green
 * @param b       Byte value for blue
 * @param cause   (optional) `String` reason why event is sent (default = `"PHYSICAL_INTERACTION"`)
 * @return the success of sending the even
 * @retval true   event has been sent successfully
 * @retval false  event has not been sent, maybe you sent to much events in a short distance of time
 **/
template <typename T>
bool ColorController<T>::sendColorEvent(byte r, byte g, byte b, String cause) {
  T& device = static_cast<T&>(*this);

  DynamicJsonDocument eventMessage = device.prepareEvent("setColor", cause.c_str());
  JsonObject event_color = eventMessage["payload"]["value"].createNestedObject("color");
  event_color["r"] = r;
  event_color["g"] = g;
  event_color["b"] = b;
  return device.sendEvent(eventMessage);
}

template <typename T>
bool ColorController<T>::handleColorController(const String &action, JsonObject &request_value, JsonObject &response_value) {
  T &device = static_cast<T &>(*this);

  bool success = false;

  if (colorCallback && action == "setColor") {
    unsigned char r, g, b;
    r = request_value["color"]["r"];
    g = request_value["color"]["g"];
    b = request_value["color"]["b"];
    success = colorCallback(device.getDeviceId(), r, g, b);
    response_value.createNestedObject("color");
    response_value["color"]["r"] = r;
    response_value["color"]["g"] = g;
    response_value["color"]["b"] = b;
  }

  return success;
}

#endif