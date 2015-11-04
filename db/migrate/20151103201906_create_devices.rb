class CreateDevices < ActiveRecord::Migration
  def change
    create_table :devices do |t|
      t.string :hardware_id
      t.text :specifications
      t.text :config
      t.text :public_key
      t.string :label
      t.string :type

      # t.timestamps null: false
    end
  end
end
