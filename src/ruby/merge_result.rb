if ARGV.length != 2 then
    print "usage: ruby merge_result.rb cadical_output_file cnf_file\n"
    exit 1
end


literal_map={}
result=open(ARGV[0],"r").readlines.find_all{|line| line=~/^v/}.collect{|line| line.scan(/-?[0-9]+/)}.flatten.collect{|literal| literal.to_i}.each{|l|literal_map[l>0?l:-l]=(l>0?1:0)}
cnf=open(ARGV[1],"r").readlines.find_all{|line| line=~/^cv/}
cnf.each{|line|
    literal_number=line.gsub(/cv <.*>/,"").to_i
    print line.chomp," #{literal_map[literal_number]}\n"
}
return 0