# encoding: UTF-8

if File.mtime("litery.ts-desc") >= File.mtime("litery.ts")
  puts 'Creating training data'
  `./io_data_gen litery.ts-desc litery.ts`
end

if File.mtime("litery_testowe.ts-desc") >= File.mtime("litery_testowe.ts")
  puts 'Creating testing data'
  `./io_data_gen litery_testowe.ts-desc litery_testowe.ts`
end

#puts 'Creating net'
#`./create_net litery.ts litery.net 1.5 32 16`

loop do
  `./train litery.net litery.net litery.ts 0.001`
  error = `./test_net litery.net litery_testowe.ts`.to_f
  puts "#{error}"
end
