package com.zeromake.onscripter;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class GameAdapter extends ArrayAdapter<Game> implements ListAdapter {
    private View.OnClickListener mOnConfigClickListener;
    private View.OnClickListener mOnPlayClickListener;
    private int selectedPos;

    public GameAdapter(Context context, ArrayList<Game> arrayList) {
        super(context, R.layout.gamelist_item, arrayList);
        this.selectedPos = -1;
    }

    @SuppressLint("InflateParams")
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        View view;
        if (convertView == null) {
            view = LayoutInflater.from(getContext()).inflate(R.layout.gamelist_item, null);
            holder = new ViewHolder(view);
            view.setTag(holder);
        } else {
            view = convertView;
            holder = (ViewHolder) view.getTag();
        }
        Game item = getItem(position);
        holder.caption.setText(item.title);
        if (position == selectedPos) {
            holder.startPanel.setVisibility(View.VISIBLE);
            holder.startPanel.setClickable(true);
            holder.icon.setImageResource(R.drawable.game_controller_1);
            holder.caption.setTextColor(getContext().getResources().getColor(R.color.sao_white));
            view.setBackgroundColor(getContext().getResources().getColor(R.color.sao_orange));
        } else {
            holder.startPanel.setVisibility(View.GONE);
            holder.startPanel.setClickable(false);
            holder.icon.setImageResource(R.drawable.game_controller_0);
            holder.caption.setTextColor(getContext().getResources().getColor(R.color.sao_grey));
            view.setBackgroundColor(getContext().getResources().getColor(R.color.sao_transparent_white));
        }
        return view;
    }
    public void setSelectedPosition(int i) {
        this.selectedPos = i;
        notifyDataSetChanged();
    }

    public int getSelectedPosition() {
        return this.selectedPos;
    }

    public Game getSelectedItem() {
        if (this.selectedPos == -1) {
            return null;
        }
        return getItem(this.selectedPos);
    }

    public void setOnConfigClickListener(View.OnClickListener onClickListener) {
        this.mOnConfigClickListener = onClickListener;
    }

    public void setOnPlayClickListener(View.OnClickListener onClickListener) {
        this.mOnPlayClickListener = onClickListener;
    }

    class ViewHolder {
        public View startPanel;
        public View btnPlay;
        public View btnConfig;
        public TextView caption;
        public ImageView icon;
        public ViewHolder(View view) {
            startPanel = view.findViewById(R.id.start_panel);
            btnPlay = view.findViewById(R.id.btn_play);
            btnConfig = view.findViewById(R.id.btn_config);
            caption = view.findViewById(R.id.caption);
            icon = view.findViewById(R.id.icon);
            if (GameAdapter.this.mOnPlayClickListener != null) {
                btnPlay.setOnClickListener(GameAdapter.this.mOnPlayClickListener);
            }
            if (GameAdapter.this.mOnConfigClickListener != null) {
                btnConfig.setOnClickListener(GameAdapter.this.mOnConfigClickListener);
            }
        }
    }
}
