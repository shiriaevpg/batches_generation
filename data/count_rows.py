filepath = "moscow_claims_2023-09-01.csv"

with open(filepath) as f:
    print(sum(1 for line in f))