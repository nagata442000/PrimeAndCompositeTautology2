require ".\\core.rb"

if ARGV.length!= 1 then
    print "usage: ruby prime_factoring_cnf.rb number."
    exit 1
end

target=ARGV[0]

if !target.match(/^(\d)+$/) then
    print "usage: ruby prime_factoring_cnf.rb number."
    exit 1
end

target=target.to_i


len=target.to_s(2).length

conditions=[
    Mul_NBit.new("factor1","factor2","target","overflow",len),
    Input_Not_Equals_Number.new("factor1",target,len),
    Input_Not_Equals_Number.new("factor2",target,len),
    Input_Equals_Number.new("target",target,len),
    "-<overflow> 0 ",
    Input_Equals_Number.new("One_NBit_#{Z(len)}",1,len),
    Input_Equals_Number.new("One_NBit_#{Z(len*2)}",1,len*2),
    " -<Zero_1Bit_#{Z(1)}> 0 "
]

generate_cnf(conditions,"prime_factoring_#{target}.cnf")
