require ".\\core.rb"

if ARGV.length!= 1 then
    print "usage: ruby prime_and_composite_tautology.rb number."
    exit 1
end

bit_width=ARGV[0]

if !bit_width.match(/^(\d)+$/) then
    print "usage: ruby prime_and_composite_tautology.rb number."
    exit 1
end

bit_width=bit_width.to_i

conditions=[
    IsPrime.new("target",bit_width),
    IsComposite.new("target",bit_width),
    Input_Equals_Number.new("One_NBit_#{Z(bit_width)}",1,bit_width),
    Input_Equals_Number.new("One_NBit_#{Z(bit_width*2)}",1,bit_width*2),
    " -<Zero_1Bit_#{Z(1)}> 0 "
]

generate_cnf(conditions,"prime_and_composite_tautology_#{bit_width}.cnf")
