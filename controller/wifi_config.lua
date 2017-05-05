module("luci.controller.admin.wifi_config", package.seeall)
function index()
	entry({"admin", "wifi_config"}, cbi("wifi_config"), _("Wifi Config"), 15).index = true
--	entry({"admin", "wifi_config"}, alias("admin", "wifi_config", "wifi_config"), _("Wifi Config"), 30).index = true
--	entry({"admin", "wifi_config", "tab1"}, cbi("wifi_config"), _("wifi ≈‰÷√"), 1)
--	entry({"admin", "wifi_config", "tab2"}, cbi("wifi_config"), _("Wifi Config2"), 2)
end


