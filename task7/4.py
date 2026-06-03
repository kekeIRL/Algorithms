def splitter(n, ans=[], c=None):
    if c == None:
        c = n
        print()
    if n < 0: return
    if n == 0: 
        print(*ans)
        return
    if c <= 0: return
    splitter(n - c, ans + [c], c)
    splitter(n, ans, c - 1)