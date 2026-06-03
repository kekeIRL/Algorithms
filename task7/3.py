def _max(nums: list[int]):
    if len(nums) == 1: return nums[0]
    pivot = len(nums) // 2
    return max( _max(nums[pivot:]), _max(nums[:pivot]))
def _min(nums: list[int]):
    if len(nums) == 1: return nums[0]
    pivot = len(nums) // 2
    return min( _min(nums[pivot:]), _min(nums[:pivot]))
def min_max(nums):
    return _min(nums), _max(nums)
