local g_bHasInit = false



---事件---


---方法---
function OnShowPanel(self, bShow)
	if not g_bHasInit then
		InitAdltWebFilter(self)
	end		
end


-------------------------
function InitAdltWebFilter(self)
	g_bHasInit = true
	return true
end


