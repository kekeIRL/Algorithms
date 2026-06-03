class TreeNode:
    def __init__(self, val, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

def _is_mirror(l, r):
    if not(l) or not(r):
        return not(l) and not(r)
    if l.val != r.val: return False
    return _is_mirror(l.left, r.right) and _is_mirror(l.right, r.left)
def is_ambigram(root):
    if not root:
        return True
    return _is_mirror(root.left, root.right)