require 'yaml'
require 'active_record'

namespace :db do
  def camel_case(str)
    return str if str !~ /_/ && str =~ /[A-Z]+.*/
    str.split('_').map{|e| e.capitalize}.join
  end

  task :environment do
    CONFIG_FILE = ENV['CONFIG'] || 'config'
    DATABASE_ENV = ENV['DATABASE_ENV'] || 'development'
    MIGRATIONS_DIR = ENV['MIGRATIONS_DIR'] || 'db/migrate'
  end

  desc 'Create a migration file with the given name'
  task :create_migration => :environment do
    desc_name = ENV['NAME'].gsub(" ", "_")
    class_name = camel_case(desc_name)
    timestamp = Time.now.strftime("%Y%m%d%H%M%S")
    file_name = timestamp + "_" + desc_name + ".rb"
    content = <<EOF
class #{class_name} < ActiveRecord::Migration
  def change
  end
end
EOF
    File.write(File.join(MIGRATIONS_DIR, file_name), content)
  end

  task :configuration => :environment do
    @domoio_config = {}
    File.open(CONFIG_FILE) do |file|
      file.each_line do |line|
        if line.include?('=')
          row = line.split('=')
          @domoio_config[row.first.to_sym] = row.last.strip
        end
      end
    end

    @config = { 'adapter' => 'postgresql' }
    @config['database'] = @domoio_config[:db_name]
    @config['username'] = @domoio_config[:db_user]
    @config['password'] = @domoio_config[:db_password]
    @config['host'] = @domoio_config[:db_host] || 'localhost'
  end

  task :configure_connection => :configuration do
    ActiveRecord::Base.establish_connection @config
    ActiveRecord::Base.logger = Logger.new STDOUT if @config['logger']
  end

  desc 'Drops the database for the current DATABASE_ENV'
  task :drop => :configure_connection do
    ActiveRecord::Base.connection.drop_database @config['database']
  end

  desc 'Migrate the database (options: VERSION=x, VERBOSE=false).'
  task :migrate => :configure_connection do
    ActiveRecord::Migration.verbose = true
    ActiveRecord::Migrator.migrate MIGRATIONS_DIR, ENV['VERSION'] ? ENV['VERSION'].to_i : nil
  end

  desc 'Rolls the schema back to the previous version (specify steps w/ STEP=n).'
  task :rollback => :configure_connection do
    step = ENV['STEP'] ? ENV['STEP'].to_i : 1
    ActiveRecord::Migrator.rollback MIGRATIONS_DIR, step
  end

  desc "Retrieves the current schema version number"
  task :version => :configure_connection do
    puts "Current version: #{ActiveRecord::Migrator.current_version}"
  end
end
