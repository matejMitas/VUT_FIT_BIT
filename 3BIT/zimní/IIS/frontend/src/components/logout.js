import React, {Component} from 'react';
import {Redirect} from "react-router-dom";
import axios from 'axios';
//
import {ROUTE_PREFIX, BASKET_NAME, API_BASE, TOKEN_NAME, LOG_OUT} from '../const.js'

export default class Logout extends Component {
    constructor(props) {
        super(props)
        this.state = {
            logoutSuccess: false
        }
    }

    componentDidMount() {
        this.logout()
    }

    logout = async () => {
        // kdyz nas server vyhodi
        this.props.onHandleAuth(LOG_OUT)
        localStorage.removeItem(TOKEN_NAME)
        localStorage.removeItem(BASKET_NAME)
        // kdyz pokracujeme dal
        try {
            this.setState({logoutSuccess: true})
            // smazeme z backendu
            await axios({
                method: 'get',
                url: `${API_BASE}logout`,
                headers: {
                    token: JSON.parse(localStorage.getItem(TOKEN_NAME))['token']
                }
            })
        } catch (error) {
            
        }
        // localStorage.removeItem(TOKEN_NAME)
        // this.setState({logoutSuccess: true})
    }

    render() {
        if (this.state.logoutSuccess) {
            return <Redirect to={`${ROUTE_PREFIX}login`}/>
        } else {
            return <div></div>
        }
    }
}