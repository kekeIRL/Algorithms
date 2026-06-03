def exchange(m, coins: list[int]):
    if m == 0: return 1
    if m < 0 or not coins: return 0
    return exchange(m - coins[0], coins) + exchange(m, coins[1:])