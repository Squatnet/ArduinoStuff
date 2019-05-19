extends Control
var loadScreenSteps = 0
var currloadStep = 1
var increment
func _process(delta):
	$Loadscreen/Sprite.rotate(0.011)
func isFinished():
	if currloadStep < loadScreenSteps:
		return false
	else:
		return true
func removeSelf():
	self.queue_free()
func loadScreenSetVars(step,num):
	print("Load screen opened "+str(step)+" steps")
	$Loadscreen/ProgressBar.max_value = step
	$Loadscreen/ProgressBar.step = num
func loadScreenStep():
	print("LoadScreen Step")
	$Loadscreen/ProgressBar.step += 1
	$Loadscreen/ProgressBar.value += 1